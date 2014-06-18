#include "JPDF.h"
#include "JGerber.h"
//------------------------------------------------------------------------------

#define White 1.0, 1.0, 1.0
#define Black 0.0, 0.0, 0.0
//------------------------------------------------------------------------------

JPDF pdf;
bool Negative = false;

struct APERTURE{
 pdfForm*  Aperture;
 APERTURE* Next;
};
APERTURE* ApertureStack = 0;
APERTURE* TempApertureStack;

pdfForm* Apertures[1000];
pdfForm* CurrentAperture = 0;
bool     SolidCircle     = false;
bool     SolidRectangle  = false;
bool     OutlinePath     = false;
double   LineWidth       = 0.0;
double   RectW           = 0.0;
double   RectH           = 0.0;
double   RectX           = 0.0;
double   RectY           = 0.0;
//------------------------------------------------------------------------------

void DrawAperture(pdfContents* Contents, GerberRender* Render){
 Contents->Push     ();
 Contents->LineCap  (pdfContents::csRound);
 Contents->LineJoin (pdfContents::jsRound);

 while(Render){
  switch(Render->Command){
   case gcRectangle:
    Contents->Rectangle(Render->X, Render->Y, Render->W, Render->H);
    break;

   case gcCircle:
    Contents->Circle(Render->X, Render->Y, Render->W/2.0);
    break;

   case gcBeginLine:
    Contents->BeginLine(Render->X, Render->Y);
    break;

   case gcLine:
    Contents->Line(Render->X, Render->Y);
    break;

   case gcArc:
    Contents->Arc(Render->X, Render->Y, Render->A);
    break;

   case gcClose:
    Contents->Close();
    break;

   case gcStroke:
    Contents->Stroke();
    break;

   case gcFill:
    Contents->Fill();
    break;

   default:
    printf("Error: Unrecognised Aperture Render Command %d\n", Render->Command);
    break;
  }
  Render = Render->Next;
 }

 Contents->Pop();

 Contents->Deflate();
}
//------------------------------------------------------------------------------

void DrawRectLine(
 pdfContents* Contents,
 double x1, double y1, // Start
 double x2, double y2, // End
 double w , double h   // Rect Width; Height
){
 w /= 2.0;
 h /= 2.0;

 if(x2 > x1){
  if(y2 > y1){
   Contents->BeginLine(x1-w, y1-h);
   Contents->     Line(x1+w, y1-h);
   Contents->     Line(x2+w, y2-h);
   Contents->     Line(x2+w, y2+h);
   Contents->     Line(x2-w, y2+h);
   Contents->     Line(x1-w, y1+h);

  }else if(y1 > y2){
   Contents->BeginLine(x1-w, y1-h);
   Contents->     Line(x2-w, y2-h);
   Contents->     Line(x2+w, y2-h);
   Contents->     Line(x2+w, y2+h);
   Contents->     Line(x1+w, y1+h);
   Contents->     Line(x1-w, y1+h);

  }else{ // y1 = y2
   Contents->BeginLine(x1-w, y1-h);
   Contents->     Line(x2+w, y2-h);
   Contents->     Line(x2+w, y2+h);
   Contents->     Line(x1-w, y1+h);
  }

 }else if(x1 > x2){
  if(y2 > y1){
   Contents->BeginLine(x2-w, y2-h);
   Contents->     Line(x1-w, y1-h);
   Contents->     Line(x1+w, y1-h);
   Contents->     Line(x1+w, y1+h);
   Contents->     Line(x2+w, y2+h);
   Contents->     Line(x2-w, y2+h);

  }else if(y1 > y2){
   Contents->BeginLine(x2-w, y2-h);
   Contents->     Line(x2+w, y2-h);
   Contents->     Line(x1+w, y1-h);
   Contents->     Line(x1+w, y1+h);
   Contents->     Line(x1-w, y1+h);
   Contents->     Line(x2-w, y2+h);

  }else{ // y1 = y2
   Contents->BeginLine(x2-w, y2-h);
   Contents->     Line(x1+w, y1-h);
   Contents->     Line(x1+w, y1+h);
   Contents->     Line(x2-w, y2+h);
  }

 }else{ // x1 = x2
  if(y2 > y1){
   Contents->BeginLine(x1-w, y1-h);
   Contents->     Line(x1+w, y1-h);
   Contents->     Line(x2+w, y2+h);
   Contents->     Line(x2-w, y2+h);

  }else{ // y1 >= y2
   Contents->BeginLine(x2-w, y2-h);
   Contents->     Line(x2+w, y2-h);
   Contents->     Line(x1+w, y1+h);
   Contents->     Line(x1-w, y1+h);
  }
 }

 Contents->Close();
 Contents->Fill ();
}
//------------------------------------------------------------------------------

void Pause(){
 printf("\nPress Enter to continue\n");
 char c;
 scanf("%c", &c);
}
//------------------------------------------------------------------------------

int RenderLayer(
 pdfPage*     Page,
 pdfContents* Contents,
 GerberLevel* Level
){
 GerberRender*   Render   = 0;
 GerberAperture* Aperture = 0;

 JString  String;

 if(Level->Negative){
  if(Negative){
   Contents->StrokeColour(Black);
   Contents->FillColour  (Black);
  }else{
   Contents->StrokeColour(White);
   Contents->FillColour  (White);
  }
 }else{
  if(Negative){
   Contents->StrokeColour(White);
   Contents->FillColour  (White);
  }else{
   Contents->StrokeColour(Black);
   Contents->FillColour  (Black);
  }
 }

 Render = Level->Render();

 while(Render){
  switch(Render->Command){
   case gcRectangle:
    Contents->Rectangle(
     Render->X,
     Render->Y,
     Render->W,
     Render->H
    );
    break;

   case gcCircle:
    Contents->Circle(Render->X, Render->Y, Render->W/2.0);
    break;

   case gcBeginLine:
    if(OutlinePath){
     Contents->BeginLine(Render->X, Render->Y);

    }else if(SolidCircle){
     Contents->LineWidth(LineWidth);
     Contents->BeginLine(Render->X, Render->Y);

    }else if(SolidRectangle){
     RectX = Render->X;
     RectY = Render->Y;

    }else{
     printf(
      "Error: Only solid circular or rectangular "
      "apertures can be used for paths\n"
     );
     Pause();
     return 1;
    }
    break;

   case gcLine:
    if(OutlinePath || SolidCircle){
     Contents->Line(Render->X, Render->Y);

    }else if(SolidRectangle){
     DrawRectLine(
      Contents,
      RectX    , RectY,
      Render->X, Render->Y,
      RectW    , RectH
     );
     RectX = Render->X;
     RectY = Render->Y;

    }else{
     printf(
      "Error: Only solid circular or rectangular "
      "apertures can be used for paths\n"
     );
     Pause();
     return 2;
    }
    break;

   case gcArc:
    if(OutlinePath || SolidCircle){
     Contents->Arc(Render->X, Render->Y, Render->A);
    }else{
     printf(
      "Error: Only solid circular apertures can be used for arcs\n"
     );
     Pause();
     return 3;
    }
    break;

   case gcFlash:
    if(CurrentAperture){
     Contents->Push();
     Contents->Translate(Render->X, Render->Y);
     Contents->Form(CurrentAperture);
     Contents->Pop();
    }else{
     printf("Error: No Aperture selected\n");
     Pause();
     return 4;
    }
    break;

   case gcClose:
    Contents->Close();
    break;

   case gcStroke:
    Contents->Stroke();
    break;

   case gcFill:
    Contents->FillEvenOdd();
    break;

   case gcBeginOutline:
    OutlinePath = true;
    break;

   case gcEndOutline:
    OutlinePath = false;
    break;

   case gcApertureSelect:
    Aperture = Render->Aperture;
    if(Aperture){
     SolidCircle    = Aperture->SolidCircle();
     LineWidth      = Aperture->Right - Aperture->Left;

     SolidRectangle = Aperture->SolidRectangle();
     RectW          = Aperture->Right - Aperture->Left;
     RectH          = Aperture->Top   - Aperture->Bottom;

     if(Apertures[Aperture->Code]){
      CurrentAperture = Apertures[Aperture->Code];
     }else{
      String.Set   ('D');
      String.Append(Aperture->Code);
      CurrentAperture = new pdfForm(String.String);
      CurrentAperture->BBox.Set(
       Aperture->Left,
       Aperture->Bottom,
       Aperture->Right,
       Aperture->Top
      );
      DrawAperture(CurrentAperture, Aperture->Render());
      Page->Resources.AddForm      (CurrentAperture);
      pdf.  AddIndirect            (CurrentAperture);
      Apertures[Aperture->Code]   = CurrentAperture;
      TempApertureStack           = new APERTURE;
      TempApertureStack->Aperture = CurrentAperture;
      TempApertureStack->Next     = ApertureStack;
      ApertureStack               = TempApertureStack;
     }
    }else{
     printf("Error: Null Aperture\n");
     Pause();
     return 5;
    }
    break;

   default:
    break;
  }
  Render = Render->Next;
 }

 return 0;
}
//------------------------------------------------------------------------------

int main(int argc, char** argv){
 int     j;
 int     PageCount = 0;
 int     BoardLayer;
 int     Result;
 double  x, y, w, h, w2, h2;
 JString OutputFileName;
 JString FileName;
 JString LayerName;

 int OptionsCount = 0;
 while(OptionsCount+1 < argc && argv[OptionsCount+1][0] == '-'){
  OptionsCount++;
  if(
   argv[OptionsCount][1] == 'o' &&
   argv[OptionsCount][2] == 'u' &&
   argv[OptionsCount][3] == 't' &&
   argv[OptionsCount][4] == 'p' &&
   argv[OptionsCount][5] == 'u' &&
   argv[OptionsCount][6] == 't' &&
   argv[OptionsCount][7] == '='
  ){
   OutputFileName.Set(argv[OptionsCount]+8);
  }
 }

 if(argc + OptionsCount < 2){
  printf(
   "Gerber2PDF, Version 1.0\n"
   "Built on "__DATE__" at "__TIME__"\n"
   "\n"
   "Copyright \xA9 John-Philip Taylor\n"
   "jpt13653903@gmail.com\n"
   "\n"
   "This program is free software: you can redistribute it and/or modify\n"
   "it under the terms of the GNU General Public License as published by\n"
   "the Free Software Foundation, either version 3 of the License, or\n"
   "(at your option) any later version.\n"
   "\n"
   "This program is distributed in the hope that it will be useful,\n"
   "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
   "GNU General Public License for more details.\n"
   "\n"
   "You should have received a copy of the GNU General Public License\n"
   "along with this program.  If not, see <http://www.gnu.org/licenses/>\n"
   "\n"
   "Usage: Gerber2pdf [-output=output_file_name] file_1 file_2 ... file_N\n"
   "\n"
   "Example: Gerber2pdf -output=My_Project top_silk.grb bottom_silk.grb\n"
   "         top_copper.grb inner_copper_1.grb\n"
   "         inner_copper_2.grb bottom_copper.grb\n"
   "         bottom_solder_mask.grb top_solder_mask.grb board_outline.grb\n"
  );
  Pause();
  return 0;
 }

 // Gerber Variables and Structures
 JGerber      Gerber;
 GerberLevel* Level;

 // PDF Variables and Structures
 pdfPages    Pages;    // Single level page tree
 pdfOutlines Outlines; // Single level outline tree

 struct LAYER_FORM{
  pdfForm*    Layer;
  LAYER_FORM* Next;

  LAYER_FORM(){
   Layer = 0;
   Next  = 0;
  }

  ~LAYER_FORM(){
   if(Layer) delete Layer;
   if(Next ) delete Next;
  }
 };
 LAYER_FORM* LayerStack = 0;
 LAYER_FORM* TempLayerStack;
 int         LayerCount = 0;

 pdfPage*         Page;     // Page for each gerber file
 pdfContents*     Contents; // Contents for each page
 pdfOutlineItems* Outline;  // Outline item for each page

 JString Title;

 char Path[0x100];
 GetCurrentDirectory(0x100, Path);
 for(j = 0; Path[j]; j++);
 if(Path[j-1] != '\\'){
  Path[j++] = '\\';
  Path[j  ] = 0;
 }

 Page     = new pdfPage        [argc-1-OptionsCount];
 Outline  = new pdfOutlineItems[argc-1-OptionsCount];
 Contents = new pdfContents    [argc-1-OptionsCount];

 // For each gerber file...
 for(BoardLayer = 0; BoardLayer < argc-1-OptionsCount; BoardLayer++){
  // Clear the variables
  for(j = 0; j < 1000; j++){
   Apertures[j] = 0;
  }

  // Read the gerber
  FileName.Set(argv[BoardLayer+1+OptionsCount]);
  Title.Set(FileName.String);
  if(FileName.GetLength() < 2) continue;
  if(FileName.String[1] != '\\' && FileName.String[1] != ':'){
   FileName.Prefix(Path);
  }

  printf("\nInfo: Converting %s\n", FileName.String);

  Gerber.Clear();
  if(!Gerber.LoadGerber(FileName.String)){
   printf("Info: There were errors while reading the gerber\n");
   continue;
  }

  if(Gerber.Name){
   Title.Set(Gerber.Name);
  }else{
   for(
    j  =      Title.GetLength()-1;
    j >= 0 && Title.String[j] != '\\';
    j--
   );
   Title.Set(Title.String+j+1);
  }

  // Write the PDF

  Outline[BoardLayer].Title.Set(Title   .String    );
  Outline[BoardLayer].DestFit  (Page    +BoardLayer);
  Page   [BoardLayer].Contents (Contents+BoardLayer);

  pdf     .AddIndirect(Page    +BoardLayer);
  pdf     .AddIndirect(Outline +BoardLayer);
  pdf     .AddIndirect(Contents+BoardLayer);
  Pages   .AddChild   (Page    +BoardLayer);
  Outlines.AddChild   (Outline +BoardLayer);
  PageCount++;

  Contents[BoardLayer].LineCap (pdfContents::csRound);
  Contents[BoardLayer].LineJoin(pdfContents::jsRound);
  Contents[BoardLayer].Use_mm();

  x  =     (Gerber.Right + Gerber.Left  )/2.0;
  y  =     (Gerber.Top   + Gerber.Bottom)/2.0;
  w  = 1.1*(Gerber.Right - Gerber.Left  );
  h  = 1.1*(Gerber.Top   - Gerber.Bottom);
  w2 = w/2.0;
  h2 = h/2.0;
  if(Gerber.Negative){
   Contents[BoardLayer].FillColour(Black);
   Contents[BoardLayer].Rectangle(x-w2, y-h2, w, h);
   Contents[BoardLayer].Fill();
   Negative = true;
  }else{
   Negative = false;
  }

  Page[BoardLayer].MediaBox.Set_mm(x-w2, y-h2, x+w2, y+h2);

  Level = Gerber.Levels;
  while(Level){
   if(!Level->Render()){
    Level = Level->Next;
    continue;
   }

   if(Level->CountX > 1 ||
      Level->CountY > 1 ){
    LayerName.Set   ("L");
    LayerName.Append(++LayerCount);
    if(Level->Name){
     LayerName.Append('_');
     LayerName.Append(Level->Name);
    }
    TempLayerStack        = new LAYER_FORM;
    TempLayerStack->Layer = new pdfForm(LayerName.String);
    TempLayerStack->Next  = LayerStack;
    LayerStack            = TempLayerStack;

    LayerStack->Layer->BBox.Set(
     Level->Left,
     Level->Bottom,
     Level->Right,
     Level->Top
    );

    Result = RenderLayer(Page+BoardLayer, LayerStack->Layer, Level);
    if(Result) return Result;

    Contents[BoardLayer].Push();

    int x, y;
    for(y = 0; y < Level->CountY; y++){
     for(x = 0; x < Level->CountX; x++){
      Contents[BoardLayer].Form(LayerStack->Layer);
      Contents[BoardLayer].Translate(Level->StepX, 0.0);
     }
     Contents[BoardLayer].Translate(
      Level->StepX * -Level->CountX,
      Level->StepY);
    }

    Contents[BoardLayer].Pop();

    Page[BoardLayer].Resources.AddForm(LayerStack->Layer);
    pdf             .AddIndirect      (LayerStack->Layer);

    LayerStack->Layer->Deflate();

   }else{
    Result = RenderLayer(Page+BoardLayer, Contents+BoardLayer, Level);
    if(Result) return Result;
   }

   Level = Level->Next;
  }

  Contents[BoardLayer].Deflate();

  Page[BoardLayer].Update();
 }

 if(PageCount){
  pdf.AddIndirect(&Pages);
  pdf.AddIndirect(&Outlines);

  pdf.Catalogue.SetPages   (&Pages);
  pdf.Catalogue.SetOutlines(&Outlines);

  pdf.Author.Set("gerber2pdf");

  if(!OutputFileName.GetLength()){
   OutputFileName.Set(FileName.String);
  }else{
   if(OutputFileName.String[1] != '\\' && OutputFileName.String[1] != ':'){
    OutputFileName.Prefix(Path);
   }
  }
  OutputFileName.Append(".pdf");

  printf("\nInfo: Writing %s\n", OutputFileName.String);
  pdf.WritePDF(OutputFileName.String);

 }else{
  printf("Info: No pages to write\n");
 }

 // Cleanup
 delete[] Page;
 delete[] Outline;
 delete[] Contents;

 while(ApertureStack){
  TempApertureStack = ApertureStack;
  ApertureStack     = ApertureStack->Next;
  delete TempApertureStack->Aperture;
  delete TempApertureStack;
 }

 if(LayerStack) delete LayerStack;

 Pause();

 return 0;
}
//------------------------------------------------------------------------------
