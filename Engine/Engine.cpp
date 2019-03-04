//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of Gerber2PDF
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#include "Engine.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

ENGINE::COLOUR::COLOUR(){
  R = G = B = A = 0;
}
//------------------------------------------------------------------------------

void ENGINE::COLOUR::operator=(COLOUR& Colour){
  R = Colour.R;
  G = Colour.G;
  B = Colour.B;
  A = Colour.A;
}
//------------------------------------------------------------------------------

bool ENGINE::COLOUR::operator== (COLOUR& Colour){
  return R == Colour.R &&
         G == Colour.G &&
         B == Colour.B &&
         A == Colour.A;
}
//------------------------------------------------------------------------------

ENGINE::OPAQUE_STACK::OPAQUE_STACK(double Value, int& OpaqueCount){
  string Name;
  Name.assign(1, 'O'), Name.append(to_string(++OpaqueCount));
  Opaque = new pdfOpaque(Name.c_str());
  Next   = 0;
  Opaque->Opacity(Value);
}
//------------------------------------------------------------------------------

ENGINE::OPAQUE_STACK::~OPAQUE_STACK(){
  delete Opaque;
  if(Next) delete Next;
}
//------------------------------------------------------------------------------

ENGINE::LEVEL_FORM::LEVEL_FORM(){
  Level = 0;
  Next  = 0;
}
//------------------------------------------------------------------------------

ENGINE::LEVEL_FORM::~LEVEL_FORM(){
  if(Level) delete Level;
  if(Next ) delete Next;
}
//------------------------------------------------------------------------------

ENGINE::LAYER::LAYER(){
  Filename              = 0;
  ConvertStrokesToFills = false;
  Form                  = 0;
  Next                  = 0;
}
//------------------------------------------------------------------------------

ENGINE::LAYER::~LAYER(){
  if(Filename) delete[] Filename;
  if(Form    ) delete   Form;
  if(Next    ) delete   Next;
}
//------------------------------------------------------------------------------

ENGINE::PAGE::PAGE(PAGE* Next){
  this->Page     = new pdfPage;
  this->Contents = new pdfContents;
  this->Next     = Next;
}
//------------------------------------------------------------------------------

ENGINE::PAGE::~PAGE(){
  delete Page;
  delete Contents;
  if(Next) delete Next;
}
//------------------------------------------------------------------------------

ENGINE::OUTLINE::OUTLINE(OUTLINE* Next){
  this->Item = new pdfOutlineItems;
  this->Next = Next;
}
//------------------------------------------------------------------------------

ENGINE::OUTLINE::~OUTLINE(){
  delete Item;
  if(Next) delete Next;
}
//------------------------------------------------------------------------------

ENGINE::ENGINE(){
  ConvertStrokesToFills = false;

  PageSize = PS_Tight;

  OpaqueCount = 0;
  OpaqueStack = 0;

  Layers = 0;

  Mirror   = false;
  Negative = false;

  Light.R = 1.0; Light.G = 1.0; Light.B = 1.0; Light.A = 0.0;
  Dark .R = 0.0; Dark .G = 0.0; Dark .B = 0.0; Dark .A = 1.0;

  ApertureStack = 0;

  ApertureCount   = 0;
  CurrentAperture = 0;

  SolidCircle    = false;
  SolidRectangle = false;
  OutlinePath    = false;
  LineWidth      = 0.0;
  RectW          = 0.0;
  RectH          = 0.0;
  RectX          = 0.0;
  RectY          = 0.0;

  Opaque = new pdfOpaque("Opaque");

  PageCount     = 0;
  Combine       = false;
  NewPage       = true;
  ThePageUsed   = false;
  ThePageLeft   =  1e100;
  ThePageBottom =  1e100;
  ThePageRight  = -1e100;
  ThePageTop    = -1e100;

  LevelStack = 0;
  LevelCount = 0;

  ThePage     = 0;
  TheContents = 0;

  Page     = 0;
  Outline  = 0;

  Opaque->Opacity(1.0);
  pdf.AddIndirect(Opaque);
}
//------------------------------------------------------------------------------

ENGINE::~ENGINE(){
  if(Page   ) delete Page;
  if(Outline) delete Outline;

  delete Opaque;

  APERTURE* TempApertureStack;

  while(ApertureStack){
    TempApertureStack = ApertureStack;
    ApertureStack     = ApertureStack->Next;
    delete TempApertureStack->Aperture;
    delete TempApertureStack;
  }

  if(Layers     ) delete Layers;
  if(LevelStack ) delete LevelStack;
  if(OpaqueStack) delete OpaqueStack;
}
//------------------------------------------------------------------------------

void ENGINE::DrawAperture(
  pdfContents*  Contents,
  GerberRender* Render,
  double Left,
  double Bottom,
  double Right,
  double Top
){
  struct OBJECT{
    GerberRender* Start; // The first one
    GerberRender* End;   // The one just after the last one
    OBJECT      * Next;
  };
  OBJECT* Objects = 0;
  OBJECT* Object;

  while(Render){
    Object = new OBJECT;
    Object->Start = Render;
    Object->End   = 0;
    Object->Next  = Objects;
    Objects       = Object;
    while(Render){
      if(
        Render->Command == gcStroke ||
        Render->Command == gcFill   ||
        Render->Command == gcErase
      ){
        Render = Render->Next;
        Object->End = Render;
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
    while(Render != Object->End){
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

void ENGINE::DrawRectLine(
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

int ENGINE::RenderLayer(
  pdfForm*     Form,
  pdfContents* Contents,
  GerberLevel* Level
){
  GerberRender*   Render   = 0;
  GerberAperture* Aperture = 0;

  string    String;
  APERTURE* TempApertureStack;

  Render = Level->Render();

  if(!Render) return 0;

  Contents->Push();

  if(Level->Negative != Negative){
    Contents->Opaque      (Opaque);
    Contents->StrokeColour(Light.R, Light.G, Light.B);
    Contents->FillColour  (Light.R, Light.G, Light.B);
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
          Contents->Pop();
          return 1;
        }
        break;

      case gcLine:
        if(OutlinePath || SolidCircle){
          Contents->Line(Render->X, Render->Y);

        }else if(SolidRectangle){
          DrawRectLine(
            Contents,
            RectX           , RectY,
            Render->X, Render->Y,
            RectW           , RectH
          );
          RectX = Render->X;
          RectY = Render->Y;

        }else{
          printf(
            "Error: Only solid circular or rectangular "
            "apertures can be used for paths\n"
          );
          Contents->Pop();
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
          Contents->Pop();
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
          Contents->Pop();
          return 4;
        }
        break;

      case gcClose:
        Contents->Close();
        break;

      case gcStroke:
        if(ConvertStrokesToFills){
          Contents->Close      ();
          Contents->FillEvenOdd();
        }else{
          Contents->Stroke();
        }
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

          pdfFormArray::iterator ApertureForm = Apertures.find(Aperture->Code);
          if(ApertureForm != Apertures.end()){
            CurrentAperture = ApertureForm->second;
          }else{
            String.assign(1, 'D');
            String.append(to_string(++ApertureCount));
            CurrentAperture = new pdfForm(String.c_str());
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
            Form->Resources.AddForm      (CurrentAperture);
            pdf.AddIndirect              (CurrentAperture);
            Apertures[Aperture->Code]   = CurrentAperture;
            TempApertureStack           = new APERTURE;
            TempApertureStack->Aperture = CurrentAperture;
            TempApertureStack->Next     = ApertureStack;
            ApertureStack               = TempApertureStack;
          }
        }else{
          printf("Error: Null Aperture\n");
          Contents->Pop();
          return 5;
        }
        break;

      default:
        break;
    }
    Render = Render->Next;
  }

  Contents->Pop();
  return 0;
}
//------------------------------------------------------------------------------

ENGINE::LAYER* ENGINE::NewLayer(
  const char* Filename,
  bool        ConvertStrokesToFills,
  COLOUR&     Light
){
  static int GerberCount = 0;

  LAYER* Layer = new LAYER;

  int j;
  for(j = 0; Filename[j]; j++);
  Layer->Filename = new char[j+1];
  for(j = 0; Filename[j]; j++) Layer->Filename[j] = Filename[j];
  Layer->Filename[j] = 0;

  Layer->ConvertStrokesToFills = ConvertStrokesToFills;
  Layer->Light                 = Light;

  string FormName;
  FormName.assign(1, 'G');
  FormName.append(to_string(++GerberCount));
  Layer->Form = new pdfForm(FormName.c_str());

  Layer->Next = Layers;
  Layers      = Layer;

  return Layer;
}
//------------------------------------------------------------------------------

static bool StringsEqual(const char* s1, const char* s2){
  if(!s1 && !s2) return true;
  if(!s1 || !s2) return false;

  int j;
  for(j = 0; s1[j] && s2[j]; j++){
    if(s1[j] != s2[j]) return false;
  }
  if(s1[j] != s2[j]) return false;
  return true;
}
//------------------------------------------------------------------------------

ENGINE::LAYER* ENGINE::FindLayer(
  const char* Filename,
  bool        ConvertStrokesToFills,
  COLOUR&     Light
){
  LAYER* Layer = Layers;
  while(Layer){
    if(
      StringsEqual(Layer->Filename, Filename) &&
      Layer->ConvertStrokesToFills == ConvertStrokesToFills &&
      Layer->Light                 == Light
    ) return Layer;
    Layer = Layer->Next;
  }
  return 0;
}
//------------------------------------------------------------------------------

int ENGINE::Run(const char* FileName, const char* Title){
  int    j;
  bool   Reusing = false;
  double x, y, w, h, w2, h2;

  LAYER* Layer;
  string LevelName;

  // Gerber Variables and Structures
  JGerber      Gerber;
  GerberLevel* Level;

  LEVEL_FORM* TempLevelStack;

  Layer = FindLayer(FileName, ConvertStrokesToFills, Light);
  if(Layer){
    printf("\nInfo: Using previous conversion of %s\n", FileName);
    Reusing = true;

  }else{
    Layer = NewLayer(FileName, ConvertStrokesToFills, Light);
    Layer->Title.assign(Title);

    printf("\nInfo: Converting %s\n", FileName);

    Gerber.Clear();
    if(!Gerber.LoadGerber(FileName)){
      printf("Info: There were errors while reading the gerber\n");
      ConvertStrokesToFills = false;
      return 0;
    }
    if(Gerber.Name){
      Layer->Title.assign(Gerber.Name);
    }else{
      for(
        j  =      Layer->Title.length()-1;
        j >= 0 && Layer->Title[j] != '\\';
        j--
      );
      Layer->Title.assign(Layer->Title.c_str() + j+1);
    }
    Layer->Left     = Gerber.Left;
    Layer->Bottom   = Gerber.Bottom;
    Layer->Right    = Gerber.Right;
    Layer->Top      = Gerber.Top;
    Layer->Negative = Gerber.Negative;

    Layer->Form->BBox.Set(
      Layer->Left,
      Layer->Bottom,
      Layer->Right,
      Layer->Top
    );
  }

  // Write the PDF
  if(!ThePage || NewPage || !Combine){
    Page        = new PAGE(Page);
    ThePage     = Page->Page;
    ThePageUsed = false;
  }
  NewPage = false;

  Outline = new OUTLINE(Outline);
  Outline->Item->Title.SetUnicode(Layer->Title.c_str());
  Outline->Item->DestFit         (ThePage);
  pdf           .AddIndirect     (Outline->Item);
  Outlines      .AddChild        (Outline->Item);

  if(!ThePageUsed){
    if(TheContents) TheContents->Deflate();
    TheContents = Page->Contents;
    ThePage->Contents   (TheContents);
    pdf     .AddIndirect(ThePage    );
    pdf     .AddIndirect(TheContents);
    Pages   .AddChild   (ThePage    );
    PageCount++;

    ThePage->Resources.AddOpaque(Opaque);

    TheContents->LineCap (pdfContents::csRound);
    TheContents->LineJoin(pdfContents::jsRound);
    TheContents->Use_mm();

    ThePageUsed = true;

    ThePageLeft   =  1e100;
    ThePageBottom =  1e100;
    ThePageRight  = -1e100;
    ThePageTop    = -1e100;

    if(Light.A > 0.5){
      TheContents->Push();
        TheContents->StrokeColour(Light.R, Light.G, Light.B);
        TheContents->FillColour  (Light.R, Light.G, Light.B);

        TheContents->Opaque(Opaque);

        TheContents->Rectangle(-1e6, -1e6, 2e6, 2e6);
        TheContents->Fill();
      TheContents->Pop();
    }
  }

  x  =      (Layer->Right + Layer->Left  )/2.0;
  y  =      (Layer->Top   + Layer->Bottom)/2.0;
  w  = 1.05*(Layer->Right - Layer->Left  );
  h  = 1.05*(Layer->Top   - Layer->Bottom);
  w2 = w/2.0;
  h2 = h/2.0;

  if(Mirror){
    if(ThePageLeft  > -x-w2) ThePageLeft  = -x-w2;
    if(ThePageRight < -x+w2) ThePageRight = -x+w2;
  }else{
    if(ThePageLeft  > x-w2) ThePageLeft  = x-w2;
    if(ThePageRight < x+w2) ThePageRight = x+w2;
  }
  if(ThePageBottom > y-h2) ThePageBottom = y-h2;
  if(ThePageTop    < y+h2) ThePageTop    = y+h2;
  ThePage->MediaBox.Set_mm(
    ThePageLeft,
    ThePageBottom,
    ThePageRight,
    ThePageTop
  );

  if(Layer->Negative){
    TheContents->Push();
      TheContents->StrokeColour(Dark.R, Dark.G, Dark.B);
      TheContents->FillColour  (Dark.R, Dark.G, Dark.B);

      if(Dark.A == 1.0){
        TheContents->Opaque(Opaque);
      }else{
        OPAQUE_STACK* TempOpaque = new OPAQUE_STACK(Dark.A, OpaqueCount);
        TempOpaque->Next = OpaqueStack;
        OpaqueStack      = TempOpaque;
        pdf.AddIndirect(TempOpaque->Opaque);
        ThePage->Resources.AddOpaque(TempOpaque->Opaque);
        TheContents->Opaque(TempOpaque->Opaque);
      }

      TheContents->Rectangle(x-w2-1e6, y-h2-1e6, w+2e6, h+2e6);
      TheContents->Fill();
    TheContents->Pop();
  }

  if(!Reusing){
    Apertures.clear();

    Negative = Layer->Negative;

    Level = Gerber.Levels;
    while(Level){
      if(!Level->Render()){
        Level = Level->Next;
        continue;
      }

      if(Level->CountX > 1 ||
         Level->CountY > 1 ){
        LevelName.assign(1, 'L');
        LevelName.append(to_string(++LevelCount));
        TempLevelStack        = new LEVEL_FORM;
        TempLevelStack->Level = new pdfForm(LevelName.c_str());
        TempLevelStack->Next  = LevelStack;
        LevelStack            = TempLevelStack;

        LevelStack->Level->BBox.Set(
          Level->Left,
          Level->Bottom,
          Level->Right,
          Level->Top
        );

        LevelStack->Level->Resources.AddOpaque(Opaque);
        Apertures.clear();
        Result = RenderLayer(LevelStack->Level, LevelStack->Level, Level);
        if(Result) return Result;

        Layer->Form->Push();

        int x, y;
        for(y = 0; y < Level->CountY; y++){
          for(x = 0; x < Level->CountX; x++){
            Layer->Form->Form(LevelStack->Level);
            Layer->Form->Translate(Level->StepX, 0.0);
          }
          Layer->Form->Translate(
            Level->StepX * -Level->CountX,
            Level->StepY);
        }

        Layer->Form->Pop();

        Layer->Form->Resources.AddForm(LevelStack->Level);
        pdf         .AddIndirect      (LevelStack->Level);

        LevelStack->Level->Deflate();
        LevelStack->Level->Update();

      }else{
        Layer->Form->Resources.AddOpaque(Opaque);
        Result = RenderLayer(Layer->Form, Layer->Form, Level);
        if(Result) return Result;
      }

      Level = Level->Next;
    }
    Layer->Form->Update();
    Layer->Form->Deflate();
    pdf.AddIndirect(Layer->Form);
  }
  ThePage    ->Resources.AddForm(Layer->Form);
  ThePage    ->Update();
  TheContents->StrokeColour(Dark.R, Dark.G, Dark.B);
  TheContents->FillColour  (Dark.R, Dark.G, Dark.B);

  TheContents->Push();
    if(Dark.A == 1.0){
      TheContents->Opaque(Opaque);
    }else{
      OPAQUE_STACK* TempOpaque = new OPAQUE_STACK(Dark.A, OpaqueCount);
      TempOpaque->Next = OpaqueStack;
      OpaqueStack      = TempOpaque;
      pdf.AddIndirect(TempOpaque->Opaque);
      ThePage->Resources.AddOpaque(TempOpaque->Opaque);
      TheContents->Opaque(TempOpaque->Opaque);
    }

    if(Mirror) TheContents->Scale(-1, 1);
    TheContents->Form(Layer->Form);
  TheContents->Pop();

  ConvertStrokesToFills = false;

  return 0;
}
//------------------------------------------------------------------------------

void ENGINE::Finish(const char* OutputFileName){
  if(TheContents) TheContents->Deflate();

  if(PageSize != PS_Tight){
    PAGE*  page;
    double Left   =  1e100;
    double Bottom =  1e100;
    double Right  = -1e100;
    double Top    = -1e100;
    double Width;
    double Height;
    double Delta;

    // Calculate the extents
    page = Page;
    while(page){
      if(page->Page->MediaBox.Left  .Value < page->Page->MediaBox.Right.Value &&
         page->Page->MediaBox.Bottom.Value < page->Page->MediaBox.Top  .Value ){
        if(Left   > page->Page->MediaBox.Left  .Value)
           Left   = page->Page->MediaBox.Left  .Value;
        if(Bottom > page->Page->MediaBox.Bottom.Value)
           Bottom = page->Page->MediaBox.Bottom.Value;
        if(Right  < page->Page->MediaBox.Right .Value)
           Right  = page->Page->MediaBox.Right .Value;
        if(Top    < page->Page->MediaBox.Top   .Value)
           Top    = page->Page->MediaBox.Top   .Value;
      }
      page = page->Next;
    }
    Width  = Right - Left;
    Height = Top - Bottom;

    // Centre on standard sizes
    if(PageSize == PS_A3){
      if(Width > Height){
        Width  = 420/25.4*72.0;
        Height = 297/25.4*72.0;
      }else{
        Width  = 297/25.4*72.0;
        Height = 420/25.4*72.0;
      }
    }else if(PageSize == PS_A4){
      if(Width > Height){
        Width  = 297/25.4*72.0;
        Height = 210/25.4*72.0;
      }else{
        Width  = 210/25.4*72.0;
        Height = 297/25.4*72.0;
      }
    }else if(PageSize == PS_Letter){
      if(Width > Height){
        Width  = 11.0*72.0;
        Height =  8.5*72.0;
      }else{
        Width  =  8.5*72.0;
        Height = 11.0*72.0;
      }
    }

    Delta   = (Width - (Right - Left)) / 2.0;
    Left   -= Delta;
    Right  += Delta;

    Delta   = (Height - (Top - Bottom)) / 2.0;
    Bottom -= Delta;
    Top    += Delta;

    page = Page;
    while(page){
      page->Page->MediaBox.Set(Left, Bottom, Right, Top);
      page = page->Next;
    }
  }

  if(PageCount){
    pdf.AddIndirect(&Pages);
    pdf.AddIndirect(&Outlines);

    pdf.Catalogue.SetPages   (&Pages);
    pdf.Catalogue.SetOutlines(&Outlines);

    pdf.Author.Set("gerber2pdf");

    printf("\nInfo: Writing %s\n", OutputFileName);
    pdf.WritePDF(OutputFileName);

  }else{
    printf("Info: No pages to write\n");
  }
}
//------------------------------------------------------------------------------
