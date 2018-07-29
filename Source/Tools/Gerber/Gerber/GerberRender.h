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

#ifndef GERBERRENDER_H
#define GERBERRENDER_H
//------------------------------------------------------------------------------

#include <stdio.h>
#include "GerberEnums.h"
#include "GerberAperture.h"
//------------------------------------------------------------------------------

// Unless specified otherwise, render counter-clockwise
//------------------------------------------------------------------------------

class GerberAperture;
class GerberRender{
public:
  GerberRender();

  GERBER_COMMAND Command;

  double X, Y, W, H, A; // W and H used for D, Dx and Dy; A in degrees

  GerberAperture* Aperture; // Used for gcApertureSelect
  GerberRender*   Next; // Points to the next render command
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

