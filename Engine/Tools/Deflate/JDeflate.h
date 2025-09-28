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

#ifndef JDEFLATE_H
#define JDEFLATE_H
//------------------------------------------------------------------------------

class JDeflate{
    private:

    public:
        JDeflate();

        // Returns newly allocated memory with the resulting data.
        // Length holds the length of the input and return buffers
        unsigned char* Inflate(unsigned char* Buffer, unsigned* Length);
        unsigned char* Deflate(unsigned char* Buffer, unsigned* Length);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

