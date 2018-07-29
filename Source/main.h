//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of Gerber2PDF
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

#ifndef main_h
#define main_h
//------------------------------------------------------------------------------

#include "JPDF.h"
#include "JGerber.h"
#include "Helvetica_afm.h"
//------------------------------------------------------------------------------

bool SilentExit            = false;
bool ConvertStrokesToFills = false;
//------------------------------------------------------------------------------

double ThePageLeft   =  1e100;
double ThePageBottom =  1e100;
double ThePageRight  = -1e100;
double ThePageTop    = -1e100;
//------------------------------------------------------------------------------

struct COLOUR{
 double R, G, B, A;
 COLOUR(){
  R = G = B = 0.0; A = 1.0;
 }
 COLOUR(double R, double G, double B, double A){
  this->R = R;
  this->G = G;
  this->B = B;
  this->A = A;
 }
};
//------------------------------------------------------------------------------

struct META_TEXT{ // For header / footer
 double  Size;  // Font height in mm
 double  Space; // Spacing, in mm
 COLOUR  Colour;
 JString Text;
};
META_TEXT Header, Footer;
bool HaveHeader, HaveFooter;
//------------------------------------------------------------------------------

int OpaqueCount = 0;
struct OPAQUE_STACK{
 pdfOpaque*    Opaque;
 OPAQUE_STACK* Next;

 OPAQUE_STACK(double Value){
  JString Name;
  Name.Set('O'), Name.Append(++OpaqueCount);
  Opaque = new pdfOpaque(Name.String);
  Next   = 0;
  Opaque->Opacity(Value);
 }

 ~OPAQUE_STACK(){
  delete Opaque;
  if(Next) delete Next;
 }
};
OPAQUE_STACK* OpaqueStack = 0;
//------------------------------------------------------------------------------

struct APERTURE{
 pdfForm*  Aperture;
 APERTURE* Next;
};
//------------------------------------------------------------------------------

struct LEVEL_FORM{
 pdfForm*    Level;
 LEVEL_FORM* Next;

 LEVEL_FORM(){
  Level = 0;
  Next  = 0;
 }

 ~LEVEL_FORM(){
  if(Level) delete Level;
  if(Next ) delete Next;
 }
};
//------------------------------------------------------------------------------

struct LAYER{
 char*    Filename;
 pdfForm* Form;
 LAYER*   Next;

 bool    Negative;
 double  Left, Bottom, Right, Top;
 JString Title;

 LAYER(){
  Filename  = 0;
  Form      = 0;
  Next      = 0;
 }

 ~LAYER(){
  if(Filename) delete[] Filename;
  if(Form    ) delete   Form;
  if(Next    ) delete   Next;
 }
};
LAYER* Layers = 0; // Stack of layer XObjects
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
