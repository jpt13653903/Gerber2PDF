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

#ifndef GERBERAPERTURE_H
#define GERBERAPERTURE_H
//------------------------------------------------------------------------------

#include <math.h>
//------------------------------------------------------------------------------

#include "GerberMacro.h"
#include "GerberRender.h"
//------------------------------------------------------------------------------

class GerberMacro;
class GerberRender;
class GerberAperture{
private:
  enum TYPE{
    tCircle,
    tRectangle,
    tObround,
    tPolygon,
    tMacro
  };

  TYPE Type;

  GerberRender* RenderList;
  GerberRender* LastRender;

  void Add(GerberRender* Render);

  void RenderHole();
  void RenderObround();
  void RenderCircle();
  void RenderPolygon();
  void RenderRectangle();

  void RenderAperture();

  // Standard Aperture (not Custom) modifiers
  double DimensionX; // Also used for outside diameter of circles
  double DimensionY;
  double HoleX; // For no hole, make this negative
  double HoleY; // For a round hole make this negative

  // Regular Polygon additional modifiers
  int    SideCount; // Number of sides
  double Rotation;  // Degrees of rotaion (rotate the whole thing CCW)

public:
  GerberAperture();
 ~GerberAperture();

  // Image bounding box
  double Left;
  double Bottom;
  double Right;
  double Top;

  int  Code; // The code to use in the D-Code to load this tool

  // Standard Aperture Generators
  void Circle   (double d);
  void Rectangle(double w, double h);
  void Obround  (double w, double h);
  void Polygon  (double w, int    n, double a); // Width; Side count; Rotation

  // Holes
  void HoleCircle   (double d);
  void HoleRectangle(double w, double h);

  // ApertureMacro: Modifiers is a null-terminated array
  // The user must free Macro
  // Modifiers is copied, so the original can be reused for something else
  void UseMacro(GerberMacro* Macro, double* Modifiers, int ModifierCount);

  // Used to determine if it is a basic shape or not
  bool SolidCircle();
  bool SolidRectangle();

  // Linked list of render commands
  // Memory freed automatically
  GerberRender* Render();
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

