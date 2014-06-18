//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library that conforms to the ZLib and Deflate
// standards
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

#ifndef GerberLevel_h
#define GerberLevel_h
//------------------------------------------------------------------------------

#include <math.h>
//------------------------------------------------------------------------------

#include <stdio.h>
#include "GerberEnums.h"
#include "GerberRender.h"
//------------------------------------------------------------------------------

class GerberLevel{
private:
 GerberRender* RenderList; // Linked list of render commands
 GerberRender* LastRender; // Last render command used for easy additions

 void Add(GerberRender* Render);

 bool   Path; // Busy with path definition
 bool   OutlineFill;
 double pX, pY; // Previous point used for relative data
 double fX, fY; // First point of polygon

 double Get_mm(double Number);

 void Move ();
 void Line ();
 void Arc  ();
 void Flash();

 GerberAperture* CurrentAperture;

public:
  GerberLevel(GerberLevel* PreviousLevel);
 ~GerberLevel();

 GerberLevel* Next;

 // Image bounding box
 double Left;
 double Bottom;
 double Right;
 double Top;

 // Step-and-Repeat
 int    CountX, CountY;
 double StepX , StepY;

 void SetName(const char* Name);

 char* Name; // null for default level
 bool  Negative;
 bool  Relative;
 bool  Incremental;
 bool  Multiquadrant;

 double X, Y, I, J;

 GERBER_UNIT          Units;
 GERBER_EXPOSURE      Exposure;
 GERBER_INTERPOLATION Interpolation;

 void ApertureSelect(GerberAperture* Aperture);

 void OutlineBegin();
 void OutlineEnd  ();

 void Do(); // Performs the action based on the current parameters

 // Linked list of render commands
 // Memory freed automatically
 GerberRender* Render();
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

