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

#ifndef JSTRING_H
#define JSTRING_H
//------------------------------------------------------------------------------

#include <math.h>
#ifdef __BORLANDC__
  double _RTLENTRY _EXPFUNC round(double __x);
#endif
//------------------------------------------------------------------------------

class JString{
  private:
    int   Memory;
    int   Length;

    void Expand  (int Length);
    void Contract(int Length);

  public:
    char* String;

    JString();
   ~JString();

    int  GetLength();
    void Set      (const char* String);
    void Set      (char     c);
    void Set      (int      i);
    void Set      (unsigned i, unsigned Places); // Hexadecimal
    void Set      (double   d, unsigned Places = 9, bool Fill = false);
    void Append   (const char* String);
    void Append   (char     c);
    void Append   (int      i);
    void Append   (unsigned i, unsigned Places); // Hexadecimal
    void Append   (double   d, unsigned Places = 9, bool Fill = false);
    void Prefix   (const char* String);
    void Prefix   (char     c);
    void Prefix   (int      i);
    void Prefix   (unsigned i, unsigned Places); // Hexadecimal
    void Prefix   (double   d, unsigned Places = 9, bool Fill = false);
    void Insert   (const char* String, int Index);
    void Insert   (char     c, int Index);
    void Insert   (int      i, int Index);
    void Insert   (unsigned i, unsigned Places, int Index); // Hexadecimal
    void Insert   (double   d, unsigned Places, int Index);
    void Remove   (int      i);
    int  Compare  (const char* String);
    void Reverse  ();
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
