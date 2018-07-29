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

GerberLevel::GerberLevel(GerberLevel* PreviousLevel){
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

 Units         = guInches;
 Exposure      = geOff;
 Interpolation = giLinear;

 if(PreviousLevel){
  SetName(PreviousLevel->Name);

  Units           = PreviousLevel->Units;
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

 if(RenderList){
  LastRender->Next = Render;
 }else{
  RenderList = Render;
 }

 LastRender = Render;
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
    Temp = new GerberRender;
    Temp->Command = gcClose;
    Add(Temp);
   }
   Temp = new GerberRender;
   Temp->Command = gcFill;
   Add(Temp);

  }else{
   Temp = new GerberRender;
   Temp->Command = gcStroke;
   Add(Temp);
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

  Temp = new GerberRender;
  Temp->Command = gcBeginLine;
  Temp->X = pX;
  Temp->Y = pY;
  Add(Temp);

 }else{
  if(
   pX == Get_mm(X) && pY == Get_mm(Y) &&
   I  == 0.0       && J  == 0.0
  ) return;
 }
 Path = true;

 switch(Interpolation){
  case giLinear:
   Temp = new GerberRender;
   Temp->Command = gcLine;
   Temp->X = Get_mm(X);
   Temp->Y = Get_mm(Y);
   Add(Temp);
   break;

  case giLinear10X:
   Temp = new GerberRender;
   Temp->Command = gcLine;
   Temp->X = Get_mm(X)*10.0;
   Temp->Y = Get_mm(Y)*10.0;
   Add(Temp);
   break;

  case giLinear0_1X:
   Temp = new GerberRender;
   Temp->Command = gcLine;
   Temp->X = Get_mm(X)*0.1;
   Temp->Y = Get_mm(Y)*0.1;
   Add(Temp);
   break;

  case giLinear0_01X:
   Temp = new GerberRender;
   Temp->Command = gcLine;
   Temp->X = Get_mm(X)*0.01;
   Temp->Y = Get_mm(Y)*0.01;
   Add(Temp);
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
  if(!Multiquadrant && a < -90.001) a += 360; // [-90; 270)
 }else{ // CCW
  while(a <= 0.0) a += 360.0; // (0; 360]
  if(!Multiquadrant && a > 90.001) a -= 360; // (-270; 90]
 }

 if(Multiquadrant){
  if(fabs(a) < 0.001){
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

 Temp = new GerberRender;
 Temp->Command = gcArc;
 Temp->X = x3;
 Temp->Y = y3;
 Temp->A = a;
 Add(Temp);

 pX = Get_mm(X);
 pY = Get_mm(Y);

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

 Temp = new GerberRender;
 Temp->Command = gcFlash;
 Temp->X = pX;
 Temp->Y = pY;
 Add(Temp);

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

 Temp           = new GerberRender;
 Temp->Aperture = Aperture;
 Temp->Command  = gcApertureSelect;
 Add(Temp);

 CurrentAperture = Aperture;
}
//------------------------------------------------------------------------------

void GerberLevel::OutlineBegin(unsigned LineNumber){
 GerberRender* Temp;

 Exposure = geOff;
 Move(LineNumber);

 Temp          = new GerberRender;
 Temp->Command = gcBeginOutline;
 Add(Temp);

 OutlineFill = true;
}
//------------------------------------------------------------------------------

void GerberLevel::OutlineEnd(unsigned LineNumber){
 GerberRender* Temp;

 Exposure = geOff;
 Move(LineNumber);

 Temp           = new GerberRender;
 Temp->Command  = gcEndOutline;
 Add(Temp);

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
