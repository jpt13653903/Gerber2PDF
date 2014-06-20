#include "JPDF.h"
#include "JGerber.h"
//------------------------------------------------------------------------------

struct COLOUR{
 double R, G, B;
 COLOUR(double R, double G, double B){
  this->R = R;
  this->G = G;
  this->B = B;
 }
};
COLOUR Dark (0.0, 0.0, 0.0);
COLOUR Clear(1.0, 1.0, 1.0);
//------------------------------------------------------------------------------

JPDF pdf;
bool Negative = false;

struct APERTURE{
 pdfForm*  Aperture;
 APERTURE* Next;
};
APERTURE* ApertureStack = 0;
APERTURE* TempApertureStack;

int      ApertureCount = 0;
pdfForm* Apertures[1000];
pdfForm* CurrentAperture = 0;

bool   SolidCircle    = false;
bool   SolidRectangle = false;
bool   OutlinePath    = false;
double LineWidth      = 0.0;
double RectW          = 0.0;
double RectH          = 0.0;
double RectX          = 0.0;
double RectY          = 0.0;
//------------------------------------------------------------------------------

void DrawAperture(
 pdfContents*  Contents,
 GerberRender* Render,
 double Left,
 double Bottom,
 double Right,
 double Top
){
 struct OBJECT{
  GerberRender* Start;
  OBJECT      * Next;
 };
 OBJECT* Objects = 0;
 OBJECT* Object;

 while(Render){
  Object = new OBJECT;
  Object->Start = Render;
  Object->Next  = Objects;
  Objects       = Object;
  while(Render){
   if(
    Render->Command == gcStroke ||
    Render->Command == gcFill   ||
    Render->Command == gcErase
   ){
    GerberRender* Temp = Render;
    Render = Render->Next;
    Temp->Next = 0;
    break;
   }
   Render = Render->Next;
  }
 }

 Contents->Push    ();
 Contents->LineCap (pdfContents::csRound);
 Contents->LineJoin(pdfContents::jsRound);

 while(Objects){
  Object  = Objects;
  Objects = Objects->Next;

  Render = Object->Start;
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

    case gcErase:
     Contents->BeginLine(Left , Bottom);
     Contents->Line     (Left , Top);
     Contents->Line     (Right, Top);
     Contents->Line     (Right, Bottom);
     Contents->Close();
     Contents->Clip();
     break;

    default:
     printf("Error: Unrecognised Aperture Render Command %d\n", Render->Command);
     break;
   }
   Render = Render->Next;
  }
  delete Object;
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

 JString String;

 Render = Level->Render();

 if(!Render) return 0;

 if(Level->Negative){
  if(Negative){
   Contents->StrokeColour(Dark.R, Dark.G, Dark.B);
   Contents->FillColour  (Dark.R, Dark.G, Dark.B);
  }else{
   Contents->StrokeColour(Clear.R, Clear.G, Clear.B);
   Contents->FillColour  (Clear.R, Clear.G, Clear.B);
  }
 }else{
  if(Negative){
   Contents->StrokeColour(Clear.R, Clear.G, Clear.B);
   Contents->FillColour  (Clear.R, Clear.G, Clear.B);
  }else{
   Contents->StrokeColour(Dark.R, Dark.G, Dark.B);
   Contents->FillColour  (Dark.R, Dark.G, Dark.B);
  }
 }

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
      String.Append(':');
      String.Append(++ApertureCount);
      CurrentAperture = new pdfForm(String.String);
      CurrentAperture->BBox.Set(
       Aperture->Left,
       Aperture->Bottom,
       Aperture->Right,
       Aperture->Top
      );
      CurrentAperture->Update();
      DrawAperture(
       CurrentAperture,
       Aperture->Render(),
       Aperture->Left,
       Aperture->Bottom,
       Aperture->Right,
       Aperture->Top
      );
      Page->Resources.AddForm      (CurrentAperture);
      pdf.AddIndirect              (CurrentAperture);
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

bool GetInt(const char* Buffer, int* Index, int* Result){
 *Result = 0;
 while(Buffer[*Index]){
  if(Buffer[*Index] >= '0' && Buffer[*Index] <= '9'){
   *Result = 10*(*Result) + Buffer[*Index] - '0';

  }else if(Buffer[*Index] == ','){
   (*Index)++;
   return true;

  }else{
   return false;
  }
  (*Index)++;
 }
 return true;
}
//------------------------------------------------------------------------------

bool StringStart(const char* String, const char* Start){
 int j;
 for(j = 0; String[j] && Start[j]; j++){
  if(String[j] != Start[j]) return false;
 }
 return !Start[j];
}
//------------------------------------------------------------------------------

int main(int argc, char** argv){
 int     j;
 int     PageCount = 0;
 int     Result;
 bool    Combine = false;
 bool    ThePageUsed = false;
 double  x, y, w, h, w2, h2;
 double  ThePageLeft   =  1e100;
 double  ThePageBottom =  1e100;
 double  ThePageRight  = -1e100;
 double  ThePageTop    = -1e100;
 JString OutputFileName;
 JString FileName;
 JString LayerName;

 int arg;

 if(argc < 2){
  printf(
   "Gerber2PDF, Version 1.1\n"
   "Built on "__DATE__" at "__TIME__"\n"
   "\n"
   "Copyright (C) John-Philip Taylor\n"
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
   "Usage: Gerber2pdf [-output=output_file_name] file_1 [-combine] file_2 ...\n"
   "       [-colour=R,G,B] file_N\n"
   "\n"
   "Example: Gerber2pdf -output=My_Project\n"
   "         top_silk.grb bottom_silk.grb\n"
   "         top_copper.grb inner_copper_1.grb\n"
   "         inner_copper_2.grb bottom_copper.grb\n"
   "         bottom_solder_mask.grb top_solder_mask.grb\n"
   "         board_outline.grb\n"
   "         -combine\n"
   "         -colour=255,0,0 top_copper.grb\n"
   "         -colour=0,128,0 top_solder_mask.grb\n"
   "         -colour=0,0,255 board_outline.grb\n"
   "         -combine\n"
   "         -colour=255,0,0 bottom_copper.grb\n"
   "         -colour=0,128,0 bottom_solder_mask.grb\n"
   "         -colour=0,0,255 board_outline.grb\n"
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

 pdfPage*         Page;            // Page for each gerber file
 pdfPage*         ThePage = 0;     // Page on which to combine outputs
 pdfContents*     Contents;        // Contents for each page
 pdfContents*     TheContents = 0; // Contents of ThePage
 pdfOutlineItems* Outline;         // Outline item for each page

 JString Title;

 char Path[0x100];
 GetCurrentDirectory(0x100, Path);
 for(j = 0; Path[j]; j++);
 if(Path[j-1] != '\\'){
  Path[j++] = '\\';
  Path[j  ] = 0;
 }

 Page     = new pdfPage        [argc];
 Outline  = new pdfOutlineItems[argc];
 Contents = new pdfContents    [argc];

 // For each argument...
 for(arg = 1; arg < argc; arg++){
  // Check for options
  if(argv[arg][0] == '-'){
   if(StringStart(argv[arg]+1, "output=")){
    OutputFileName.Set(argv[arg]+8);

   }else if(StringStart(argv[arg]+1, "colour=")){
    int i = 8;
    int R, G, B;
    if(!GetInt(argv[arg], &i, &R)) continue;
    if(!GetInt(argv[arg], &i, &G)) continue;
    if(!GetInt(argv[arg], &i, &B)) continue;
    if(R < 0 || R > 255) continue;
    if(G < 0 || G > 255) continue;
    if(B < 0 || B > 255) continue;
    Dark.R = R/255.0;
    Dark.G = G/255.0;
    Dark.B = B/255.0;

   }else if(StringStart(argv[arg]+1, "combine")){
    Combine     = true;
    ThePage     = Page+arg;
    ThePageUsed = false;
   }
   continue; // handle the next argument
  }

  // Clear the variables
  for(j = 0; j < 1000; j++) Apertures[j] = 0;

  // Read the gerber
  FileName.Set(argv[arg]);
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
  if(!ThePage || !Combine){
   ThePage     = Page+arg;
   ThePageUsed = false;
  }
  Outline[arg].Title.Set  (Title.String);
  Outline[arg].DestFit    (ThePage);
  pdf         .AddIndirect(Outline+arg);
  Outlines    .AddChild   (Outline+arg);

  if(!ThePageUsed){
   TheContents = Contents+arg;
   ThePage->Contents   (TheContents);
   pdf     .AddIndirect(ThePage    );
   pdf     .AddIndirect(TheContents);
   Pages   .AddChild   (ThePage    );
   PageCount++;

   TheContents->LineCap (pdfContents::csRound);
   TheContents->LineJoin(pdfContents::jsRound);
   TheContents->Use_mm();

   ThePageUsed = true;

   ThePageLeft   =  1e100;
   ThePageBottom =  1e100;
   ThePageRight  = -1e100;
   ThePageTop    = -1e100;
  }

  x  =      (Gerber.Right + Gerber.Left  )/2.0;
  y  =      (Gerber.Top   + Gerber.Bottom)/2.0;
  w  = 1.05*(Gerber.Right - Gerber.Left  );
  h  = 1.05*(Gerber.Top   - Gerber.Bottom);
  w2 = w/2.0;
  h2 = h/2.0;
  if(Gerber.Negative){
   TheContents->FillColour(Dark.R, Dark.G, Dark.B);
   TheContents->Rectangle(x-w2, y-h2, w, h);
   TheContents->Fill();
   Negative = true;
  }else{
   Negative = false;
  }

  if(ThePageLeft   > x-w2) ThePageLeft   = x-w2;
  if(ThePageBottom > y-h2) ThePageBottom = y-h2;
  if(ThePageRight  < x+w2) ThePageRight  = x+w2;
  if(ThePageTop    < y+h2) ThePageTop    = y+h2;
  ThePage->MediaBox.Set_mm(
   ThePageLeft,
   ThePageBottom,
   ThePageRight,
   ThePageTop
  );

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

    Result = RenderLayer(ThePage, LayerStack->Layer, Level);
    if(Result) return Result;

    TheContents->Push();

    int x, y;
    for(y = 0; y < Level->CountY; y++){
     for(x = 0; x < Level->CountX; x++){
      TheContents->Form(LayerStack->Layer);
      TheContents->Translate(Level->StepX, 0.0);
     }
     TheContents->Translate(
      Level->StepX * -Level->CountX,
      Level->StepY);
    }

    TheContents->Pop();

    ThePage->Resources.AddForm(LayerStack->Layer);
    pdf     .AddIndirect      (LayerStack->Layer);

    LayerStack->Layer->Deflate();

   }else{
    Result = RenderLayer(ThePage, TheContents, Level);
    if(Result) return Result;
   }

   Level = Level->Next;
  }
//  TheContents->Deflate();
  ThePage    ->Update();
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
