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

#ifndef GERBERENUMS_H
#define GERBERENUMS_H
//------------------------------------------------------------------------------

enum GERBER_UNIT{
  guInches,
  guMillimeters
};
//------------------------------------------------------------------------------

enum GERBER_INTERPOLATION{
  giLinear,
  giLinear10X,
  giLinear0_1X,
  giLinear0_01X,
  giClockwiseCircular,
  giCounterclockwiseCircular
};
//------------------------------------------------------------------------------

enum GERBER_EXPOSURE{
  geOn,
  geOff,
  geFlash
};
//------------------------------------------------------------------------------

enum GERBER_COMMAND{
  // Begin Path
  gcRectangle,
  gcCircle,
  gcBeginLine,

  // Continue Path
  gcLine,
  gcArc,

  // Close Path
  gcClose,

  // Render Path
  gcStroke,
  gcFill,
  gcErase, // Used for "exposure off" aperture primitives

  // Other
  gcBeginOutline, // The aperture does not matter
  gcEndOutline,   // The aperture matters again
  gcApertureSelect,
  gcFlash
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
