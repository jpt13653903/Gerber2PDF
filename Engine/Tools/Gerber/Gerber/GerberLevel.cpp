//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library
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

#include "GerberLevel.h"
//------------------------------------------------------------------------------

#define pi 3.141592653589793238463
//------------------------------------------------------------------------------

GerberLevel::GerberLevel(GerberLevel* PreviousLevel, GERBER_UNIT Units){
  Name       = 0;
  RenderList = 0;
  LastRender = 0;
  Next       = 0;

  CurrentAperture = 0;

  Left   =  1e3;
  Bottom =  1e3;
  Right  = -1e3;
  Top    = -1e3;

  CountX = CountY = 1;
  StepX  = StepY  = 0.0;

  Path          = false;
  Negative      = false;
  Relative      = false;
  Incremental   = false;
  Multiquadrant = false;
  OutlineFill   = false;

  fX = fY = pX = pY = X = Y = I = J = 0.0;

  this->Units   = Units;
  Exposure      = geOff;
  Interpolation = giLinear;

  if(PreviousLevel){
    SetName(PreviousLevel->Name);

    Exposure        = PreviousLevel->Exposure;
    CurrentAperture = PreviousLevel->CurrentAperture;

    Multiquadrant = PreviousLevel->Multiquadrant;
    Interpolation = PreviousLevel->Interpolation;
    pX            = PreviousLevel->pX;
    pY            = PreviousLevel->pY;
     X            = PreviousLevel-> X;
     Y            = PreviousLevel-> Y;

    CountX = PreviousLevel->CountX;
    CountY = PreviousLevel->CountY;
    StepX  = PreviousLevel->StepX;
    StepY  = PreviousLevel->StepY;

    Negative = PreviousLevel->Negative;
    Path     = PreviousLevel->Path;
  }
}
//------------------------------------------------------------------------------

GerberLevel::~GerberLevel(){
  GerberRender* Temp;

  if(Next) delete Next;

  while(RenderList){
    Temp       = RenderList;
    RenderList = RenderList->Next;
    delete Temp;
  }

  if(Name) delete[] Name;
}
//------------------------------------------------------------------------------

void GerberLevel::SetName(const char* Name){
  int j;

  if(this->Name) delete this->Name;
  if(!Name){
    this->Name = 0;
    return;
  }

  for(j = 0; Name[j]; j++);
  this->Name = new char[j+1];
  for(j = 0; Name[j]; j++){;
    this->Name[j] = Name[j];
  }
  this->Name[j] = 0;
}
//------------------------------------------------------------------------------

double GerberLevel::Get_mm(double Number){
  if(Units == guInches) Number *= 25.4;
  return Number;
}
//------------------------------------------------------------------------------

void GerberLevel::Add(GerberRender* Render){
  Render->Next = 0;

  if(RenderList) LastRender->Next = Render;
  else           RenderList       = Render;

  LastRender = Render;
}
//------------------------------------------------------------------------------

GerberRender* GerberLevel::AddNew(GERBER_COMMAND Command){
  GerberRender* Temp = new GerberRender;
  Temp->Command = Command;
  Add(Temp);
  return Temp;
}
//------------------------------------------------------------------------------

void GerberLevel::Move(unsigned LineNumber){
  GerberRender* Temp;

  if(Path){
    if(OutlineFill){
      if(fX != pX || fY != pY){
        if(GerberWarnings) printf(
          "Line %d - Warning: Deprecated feature: Open contours\n",
          LineNumber
        );
        AddNew(gcClose);
      }
      Temp = AddNew(gcFill);
      Temp->End.X = pX;
      Temp->End.Y = pY;

    }else{
      Temp = AddNew(gcStroke);
      Temp->End.X = pX;
      Temp->End.Y = pY;
    }
  }

  Path = false;

  fX = Get_mm(X);
  fY = Get_mm(Y);
  pX = fX;
  pY = fY;
}
//------------------------------------------------------------------------------

void GerberLevel::Line(){
  GerberRender* Temp;

  double l, b, r, t;

  if(!Path){
    l = pX;
    b = pY;
    r = pX;
    t = pY;

    if(CurrentAperture && !OutlineFill){
      l += CurrentAperture->Left;
      b += CurrentAperture->Bottom;
      r += CurrentAperture->Right;
      t += CurrentAperture->Top;
    }

    if(Left   > l) Left   = l;
    if(Bottom > b) Bottom = b;
    if(Right  < r) Right  = r;
    if(Top    < t) Top    = t;

    Temp = AddNew(gcBeginLine);
    Temp->End.X = Temp->X = pX;
    Temp->End.Y = Temp->Y = pY;

  }else{
    if(
      pX == Get_mm(X) && pY == Get_mm(Y) &&
      I  == 0.0       && J  == 0.0
    ) return;
  }
  Path = true;

  switch(Interpolation){
    case giLinear:
      Temp = AddNew(gcLine);
      Temp->End.X = Temp->X = Get_mm(X);
      Temp->End.Y = Temp->Y = Get_mm(Y);
      break;

    case giLinear10X:
      Temp = AddNew(gcLine);
      Temp->End.X = Temp->X = Get_mm(X)*10.0;
      Temp->End.Y = Temp->Y = Get_mm(Y)*10.0;
      break;

    case giLinear0_1X:
      Temp = AddNew(gcLine);
      Temp->End.X = Temp->X = Get_mm(X)*0.1;
      Temp->End.Y = Temp->Y = Get_mm(Y)*0.1;
      break;

    case giLinear0_01X:
      Temp = AddNew(gcLine);
      Temp->End.X = Temp->X = Get_mm(X)*0.01;
      Temp->End.Y = Temp->Y = Get_mm(Y)*0.01;
      break;

    case giClockwiseCircular:
    case giCounterclockwiseCircular:
      Arc();
      break;

    default:
      break;
  }

  pX = Get_mm(X);
  pY = Get_mm(Y);

  l = pX;
  b = pY;
  r = pX;
  t = pY;

  if(CurrentAperture && !OutlineFill){
    l += CurrentAperture->Left;
    b += CurrentAperture->Bottom;
    r += CurrentAperture->Right;
    t += CurrentAperture->Top;
  }

  if(Left   > l) Left   = l;
  if(Bottom > b) Bottom = b;
  if(Right  < r) Right  = r;
  if(Top    < t) Top    = t;
}
//------------------------------------------------------------------------------

double GerberLevel::GetAngle(
  double x1, double y1, // Start, relative to center
  double x2, double y2  // End, relative to center
){
  double a1 = atan2(y1, x1)*180.0/pi;
  double a2 = atan2(y2, x2)*180.0/pi;

  double a = a2 - a1; // [-360; 360]

  if(Interpolation == giClockwiseCircular){ // CW
    while(a >= 0.0) a -= 360.0; // [-360; 0)
    if(!Multiquadrant && a < -95.0) a += 360; // [-95; 265)
  }else{ // CCW
    while(a <= 0.0) a += 360.0; // (0; 360]
    if(!Multiquadrant && a > 95.0) a -= 360; // (-265; 95]
  }

  if(Multiquadrant){
    if(fabs(a) < 0.002){
      if(a >= 0.0) a += 360;
      else         a -= 360;
    }
  }

  return a;
}
//------------------------------------------------------------------------------

void GerberLevel::Arc(){
  double x1, y1; // Start, relative to center
  double x2, y2; // End, relative to center
  double x3, y3; // Center

  double a, l, b, r, t;

  GerberRender* Temp;

  if(Multiquadrant){
    x3 = pX + Get_mm(I);
    y3 = pY + Get_mm(J);

    x1 = pX - x3;
    y1 = pY - y3;
    x2 = Get_mm(X) - x3;
    y2 = Get_mm(Y) - y3;

  }else{
    int    t, T;
    double i[4], j[4], error, Error;

    i[0] = Get_mm(I); j[0] = Get_mm(J);
    i[1] = -i[0]    ; j[1] =  j[0];
    i[2] = -i[0]    ; j[2] = -j[0];
    i[3] =  i[0]    ; j[3] = -j[0];

    // Find the best match for the centre coordinates
    // by minimising the error function.
    T = 0; Error = 1.0 / 0.0;
    for(t = 0; t < 4; t++){
      x3 = pX + i[t];
      y3 = pY + j[t];

      x1 = pX - x3;
      y1 = pY - y3;
      x2 = Get_mm(X) - x3;
      y2 = Get_mm(Y) - y3;

      a = GetAngle(x1, y1, x2, y2);
      if(Interpolation == giClockwiseCircular){
        if(a > 0.0) continue;
      }else{ // CCW
        if(a < 0.0) continue;
      }

      error = fabs((x1*x1 + y1*y1) - (x2*x2 + y2*y2));
      if(error < Error){
        T     = t;
        Error = error;
      }
    }
    x3 = pX + i[T];
    y3 = pY + j[T];

    x1 = pX - x3;
    y1 = pY - y3;
    x2 = Get_mm(X) - x3;
    y2 = Get_mm(Y) - y3;
  }

  a = GetAngle(x1, y1, x2, y2);

  Temp = AddNew(gcArc);
  Temp->X = x3;
  Temp->Y = y3;
  Temp->A = a;
  Temp->End.X = pX = Get_mm(X);
  Temp->End.Y = pY = Get_mm(Y);

  // Calculate bounding box
  int    j;
  double x, y, d, dd, rad;

  rad = sqrt(x1*x1 + y1*y1); // Radius

  l = r = x3 + x1;
  b = t = y3 + y1;

  d  = atan2(y1, x1);
  dd = a * pi / 180e3;
  for(j = 0; j < 1000; j++){
    x = x3 + rad*cos(d);
    y = y3 + rad*sin(d);
    if(l > x) l = x;
    if(b > y) b = y;
    if(r < x) r = x;
    if(t < y) t = y;
    d += dd;
  }

  if(CurrentAperture && !OutlineFill){
    l += CurrentAperture->Left;
    b += CurrentAperture->Bottom;
    r += CurrentAperture->Right;
    t += CurrentAperture->Top;
  }

  if(Left   > l) Left   = l;
  if(Bottom > b) Bottom = b;
  if(Right  < r) Right  = r;
  if(Top    < t) Top    = t;

  // I and J are cleared in GerberLevel::Do()
}
//------------------------------------------------------------------------------

void GerberLevel::Flash(){
  GerberRender* Temp;

  double l, b, r, t;

  // pX and pY set in call to Move()
  // pX = Get_mm(X);
  // pY = Get_mm(Y);

  Temp = AddNew(gcFlash);
  Temp->X = pX;
  Temp->Y = pY;

  if(CurrentAperture){
    l = pX + CurrentAperture->Left;
    b = pY + CurrentAperture->Bottom;
    r = pX + CurrentAperture->Right;
    t = pY + CurrentAperture->Top;

    if(Left   > l) Left   = l;
    if(Bottom > b) Bottom = b;
    if(Right  < r) Right  = r;
    if(Top    < t) Top    = t;
  }
}
//------------------------------------------------------------------------------

void GerberLevel::ApertureSelect(GerberAperture* Aperture, unsigned LineNumber){
  GerberRender* Temp;

  Exposure = geOff;
  Move(LineNumber);

  Temp = AddNew(gcApertureSelect);
  Temp->Aperture  = Aperture;
  CurrentAperture = Aperture;
}
//------------------------------------------------------------------------------

void GerberLevel::OutlineBegin(unsigned LineNumber){
  Exposure = geOff;
  Move(LineNumber);

  AddNew(gcBeginOutline);

  OutlineFill = true;
}
//------------------------------------------------------------------------------

void GerberLevel::OutlineEnd(unsigned LineNumber){
  Exposure = geOff;
  Move(LineNumber);

  AddNew(gcEndOutline);

  OutlineFill = false;
}
//------------------------------------------------------------------------------

void GerberLevel::Do(unsigned LineNumber){
  switch(Exposure){
    case geOn:
      Line();
      break;

    case geOff:
      Move(LineNumber);
      break;

    case geFlash:
      Move(LineNumber);
      Flash();
      Exposure = geOff;
      break;

    default:
      break;
  }

  I = J = 0.0;
}
//------------------------------------------------------------------------------

GerberRender* GerberLevel::Render(){
  return RenderList;
}
//------------------------------------------------------------------------------

void GerberLevel::Segment::Add(GerberRender* Command){
  Closed = false;

  if(LastCommand) LastCommand->Next = Command;
  else            CommandList       = Command;

  LastCommand = Command;
}
//------------------------------------------------------------------------------

bool GerberLevel::Segment::IsClosed(){
  if(Closed) return true;
  if(!LastCommand) return false;

  assert(CommandList->Command == gcBeginLine);
  assert(LastCommand->Command == gcLine ||
         LastCommand->Command == gcArc  );

  if(LastCommand->End.X == CommandList->X &&
     LastCommand->End.Y == CommandList->Y){
    Closed = true;
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------

void swap(double& A, double& B){
  double T = A;
  A = B;
  B = T;
}
//------------------------------------------------------------------------------

void GerberLevel::Segment::Reverse(){
  if(IsClosed()) return; // Don't bother reversing closed loops
  if(!CommandList) return;

  GerberRender* OldList = CommandList;
  CommandList = LastCommand = 0;

  GerberRender* BeginLine = OldList;
  OldList = OldList->Next;
  LastCommand = OldList;

  assert(BeginLine->Command == gcBeginLine);

  double X = BeginLine->X;
  double Y = BeginLine->Y;

  GerberRender* Command;
  while(OldList){
    Command = OldList;
    OldList = OldList->Next;
    Command->Next = 0;

    switch(Command->Command){
      case gcLine:
        Command->X = X;
        Command->Y = Y;
        swap(X, Command->End.X);
        swap(Y, Command->End.Y);
        Command->Next = CommandList;
        CommandList   = Command;
        break;

      case gcArc:
        Command->A *= -1;
        swap(X, Command->End.X);
        swap(Y, Command->End.Y);
        Command->Next = CommandList;
        CommandList = Command;
        break;

      default:
        assert(false);
        delete Command;
        break;
    }
  }
  BeginLine->X = X;
  BeginLine->Y = Y;
  BeginLine->Next = CommandList;
  CommandList = BeginLine;
}
//------------------------------------------------------------------------------

void GerberLevel::Segment::Isolate(){
  if(Prev) Prev->Next = Next;
  if(Next) Next->Prev = Prev;
  Next = Prev = 0;
}
//------------------------------------------------------------------------------

void GerberLevel::NewSegment(){
  Segment* Temp = new Segment;

  if(LastSegment) LastSegment->Next = Temp;
  else            SegmentList       = Temp;

  Temp->Prev  = LastSegment;
  LastSegment = Temp;
}
//------------------------------------------------------------------------------

void GerberLevel::ExtractSegments(){
  GerberRender* OldList = RenderList;
  RenderList = LastRender = 0;

  bool isOutline = false;

  GerberRender* Command;

  while(OldList){
    Command = OldList;
    OldList = OldList->Next;
    Command->Next = 0;

    switch(Command->Command){
      case gcBeginLine:
        if(isOutline){
          Add(Command);
        }else{
          NewSegment();
          LastSegment->Add(Command);
        }
        break;

      case gcLine:
        if(isOutline){
          Add(Command);
        }else{
          assert(LastSegment, NewSegment());
          LastSegment->Add(Command);
        }
        break;

      case gcArc:
        if(isOutline){
          Add(Command);
        }else{
          assert(LastSegment, NewSegment());
          LastSegment->Add(Command);
        }
        break;

      case gcStroke:
        assert(isOutline == false);
        delete Command;
        break;

      case gcClose:
      case gcFill:
        assert(isOutline == true);
        Add(Command);
        break;

      case gcBeginOutline:
        isOutline = true;
        Add(Command);
        break;

      case gcEndOutline:
        isOutline = false;
        Add(Command);
        break;

      case gcApertureSelect:
      case gcFlash:
        delete Command;
        break;

      case gcRectangle:
      case gcCircle:
      case gcErase:
      default:
        assert(false);
        delete Command;
        break;
    }
  }
}
//------------------------------------------------------------------------------

GerberLevel::Segment* GerberLevel::FindNeighbour(Segment* Current){
  if(!Current->CommandList) return 0;
  if( Current->IsClosed() ) return 0;

  Segment* Candidate = SegmentList;

  while(Candidate){
    if(Candidate != Current && Candidate->CommandList && !Candidate->IsClosed()){
      if(Current->LastCommand->End.X == Candidate->CommandList->X &&
         Current->LastCommand->End.Y == Candidate->CommandList->Y ){
        return Candidate;
      }
      if(Current->LastCommand->End.X == Candidate->LastCommand->End.X &&
         Current->LastCommand->End.Y == Candidate->LastCommand->End.Y ){
        Candidate->Reverse();
        return Candidate;
      }
    }
    Candidate = Candidate->Next;
  }

  // No candidates found, but run the test again checking for near segments.
  // Points are considered the same if they are closer than 1 μm.
  // This is required because many Gerber generators make rounding errors.

  Candidate = SegmentList;
  double dX, dY;

  while(Candidate){
    if(Candidate != Current && Candidate->CommandList && !Candidate->IsClosed()){
      dX = fabs(Current->LastCommand->End.X - Candidate->CommandList->X);
      dY = fabs(Current->LastCommand->End.Y - Candidate->CommandList->Y);
      if(dX < 1e-3 && dY < 1e-3 ){
        if(GerberWarnings) printf(
          "Strokes2Fills - Warning: "
          "Joining segments that are close, but not coincident:\n"
          "    dX = %08.6lf mm (%07.5lf mil)\n"
          "    dY = %08.6lf mm (%07.5lf mil)\n",
          dX, dX/25.4e-3,
          dY, dY/25.4e-3
        );
        return Candidate;
      }
      dX = fabs(Current->LastCommand->End.X - Candidate->LastCommand->End.X);
      dY = fabs(Current->LastCommand->End.Y - Candidate->LastCommand->End.Y);
      if(dX < 1e-3 && dY < 1e-3 ){
        Candidate->Reverse();
        if(GerberWarnings) printf(
          "Strokes2Fills - Warning: "
          "Joining segments that are close, but not coincident:\n"
          "    dX = %08.6lf mm (%07.5lf mil)\n"
          "    dY = %08.6lf mm (%07.5lf mil)\n",
          dX, dX/25.4e-3,
          dY, dY/25.4e-3
        );
        return Candidate;
      }
    }
    Candidate = Candidate->Next;
  }
  return 0;
}
//------------------------------------------------------------------------------

void GerberLevel::JoinSegments(){
  Segment* Current = SegmentList;
  GerberRender* BeginLine;

  while(Current){
    Segment* Neighbour = FindNeighbour(Current);

    if(Neighbour){
      if(SegmentList == Neighbour) SegmentList = SegmentList->Next;
      if(LastSegment == Neighbour) LastSegment = LastSegment->Prev;
      Neighbour->Isolate();

      BeginLine = Neighbour->CommandList;
      Neighbour->CommandList = Neighbour->CommandList->Next;
      delete BeginLine;

      Current->LastCommand->Next = Neighbour->CommandList;
      Current->LastCommand       = Neighbour->LastCommand;
      delete Neighbour;

    }else{
      Current = Current->Next;
    }
  }
}
//------------------------------------------------------------------------------

void GerberLevel::AddSegments(){
  Segment* Temp;

  if(SegmentList){
    AddNew(gcBeginOutline);

    while(SegmentList){
      assert(SegmentList->CommandList);
      assert(SegmentList->CommandList->Command == gcBeginLine);

      LastRender->Next = SegmentList->CommandList;
      LastRender       = SegmentList->LastCommand;

      if(!SegmentList->IsClosed()) AddNew(gcClose);

      Temp = SegmentList;
      SegmentList = SegmentList->Next;
      Temp->Isolate();
      delete Temp;
    }
    LastSegment = 0;

    AddNew(gcFill);
    AddNew(gcEndOutline);
  }
}
//------------------------------------------------------------------------------

void GerberLevel::ConvertStrokesToFills(){
  ExtractSegments();
  JoinSegments();
  AddSegments();
}
//------------------------------------------------------------------------------
