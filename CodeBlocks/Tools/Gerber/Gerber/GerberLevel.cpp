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

GerberLevel::GerberLevel(){
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

 Exposure           = geOff;
 Interpolation      = giLinear;
 GerberLevel::Units = guInches;
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

void GerberLevel::Move(){
 GerberRender* Temp;

 if(Path){
  if(OutlineFill){
   if(fX != X || fY != Y){
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

 pX = Get_mm(X);
 pY = Get_mm(Y);
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
  if(pX == Get_mm(X) && pY == Get_mm(Y)) return;
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

void GerberLevel::Arc(){
 double x1, y1; // Start, relative to center
 double x2, y2; // End, relative to center
 double x3, y3; // Center
 double a1, a2, a;

 double l, b, r, t;

 GerberRender* Temp;

 if(Multiquadrant){
  x3 = pX + Get_mm(I);
  y3 = pY + Get_mm(J);
 }else{
  if(Get_mm(X) > pX) x3 = pX + Get_mm(I);
  else               x3 = pX - Get_mm(I);
  if(Get_mm(Y) > pY) y3 = pY + Get_mm(J);
  else               y3 = pY - Get_mm(J);
 }

 x1 = pX - x3;
 y1 = pY - y3;

 x2 = Get_mm(X) - x3;
 y2 = Get_mm(Y) - y3;

 a1 = atan2(y1, x1)*180.0/pi;
 a2 = atan2(y2, x2)*180.0/pi;

 a = a2 - a1; // [-360; 360]
 if(a < 0.0) a += 360.0; // [0; 360]

 if(Interpolation == giClockwiseCircular){
  if(a > 0.0) a -= 360.0; // (-360; 0]
  if(!Multiquadrant && a < -90) a += 360; // [-90; 270)
 }else{
  if(!Multiquadrant && a >  90) a -= 360; // (-270; 90]
 }

 if(Multiquadrant){
  if(fabs(a) < 0.001){
   if(a >= 0.0) a += 360;
   else         a -= 360;
  }
 }

 Temp = new GerberRender;
 Temp->Command = gcArc;
 Temp->X = x3;
 Temp->Y = y3;
 Temp->A = a;
 Add(Temp);

 pX = Get_mm(X);
 pY = Get_mm(Y);

 a = sqrt(x1*x1 + y1*y1); // Radius

 l = x3-a;
 b = y3-a;
 r = x3+a;
 t = y3+a;

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

void GerberLevel::Flash(){
 GerberRender* Temp;

 double l, b, r, t;

 pX = Get_mm(X);
 pY = Get_mm(Y);

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

void GerberLevel::ApertureSelect(GerberAperture* Aperture){
 GerberRender* Temp;

 Exposure = geOff;
 Move();

 Temp           = new GerberRender;
 Temp->Aperture = Aperture;
 Temp->Command  = gcApertureSelect;
 Add(Temp);

 CurrentAperture = Aperture;
}
//------------------------------------------------------------------------------

void GerberLevel::OutlineBegin(){
 GerberRender* Temp;

 Temp           = new GerberRender;
 Temp->Command  = gcBeginOutline;
 Add(Temp);

 OutlineFill = true;
}
//------------------------------------------------------------------------------

void GerberLevel::OutlineEnd(){
 GerberRender* Temp;

 Exposure = geOff;
 Move();

 Temp           = new GerberRender;
 Temp->Command  = gcEndOutline;
 Add(Temp);

 OutlineFill = false;
}
//------------------------------------------------------------------------------

void GerberLevel::Do(){
 switch(Exposure){
  case geOn:
   Line();
   break;

  case geOff:
   Move();
   break;

  case geFlash:
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
