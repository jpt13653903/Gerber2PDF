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

#include "pdfDest.h"
//------------------------------------------------------------------------------

void pdfDest::Fit(pdfPage* Page){
 pdfDest::Page = Page;
 Style.Set("Fit");

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
}
//------------------------------------------------------------------------------

void pdfDest::FitB(pdfPage* Page){
 pdfDest::Page = Page;
 Style.Set("FitB");

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
}
//------------------------------------------------------------------------------

void pdfDest::FitH(pdfPage* Page, double Top){
 pdfDest::Page = Page;
 Style.Set("FitH");
 Number[0] = Top;

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
 Add(Number);
}
//------------------------------------------------------------------------------

void pdfDest::FitV(pdfPage* Page, double Left){
 pdfDest::Page = Page;
 Style.Set("FitV");
 Number[0] = Left;

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
}
//------------------------------------------------------------------------------

void pdfDest::FitBH(pdfPage* Page, double Top){
 pdfDest::Page = Page;
 Style.Set("FitBH");
 Number[0] = Top;

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
 Add(Number);
}
//------------------------------------------------------------------------------

void pdfDest::FitBV(pdfPage* Page, double Left){
 pdfDest::Page = Page;
 Style.Set("FitBV");
 Number[0] = Left;

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
 Add(Number);
}
//------------------------------------------------------------------------------

void pdfDest::XYZ(
 pdfPage* Page,
 double   Left,
 double   Top,
 double   Zoom
){
 pdfDest::Page = Page;
 Style.Set("XYZ");
 Number[0] = Left;
 Number[1] = Top;
 Number[2] = Zoom;

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
 Add(Number  );
 Add(Number+1);
 Add(Number+2);
}
//------------------------------------------------------------------------------

void pdfDest::FitR(
 pdfPage* Page,
 double   Left,
 double   Bottom,
 double   Right,
 double   Top
){
 pdfDest::Page = Page;
 Style.Set("FitR");
 Number[0] = Left;
 Number[1] = Bottom;
 Number[2] = Right;
 Number[3] = Top;

 Clear();
 Add(pdfDest::Page);
 Add(&Style);
 Add(Number  );
 Add(Number+1);
 Add(Number+2);
 Add(Number+3);
}
//------------------------------------------------------------------------------
