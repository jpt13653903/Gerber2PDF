//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library.
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

#ifndef JGERBER_H
#define JGERBER_H
//------------------------------------------------------------------------------

#include <stdio.h>
//------------------------------------------------------------------------------

#include "FileWrapper.h"

#include "Gerber/GerberAperture.h"
#include "Gerber/GerberLevel.h"
//------------------------------------------------------------------------------

extern bool GerberWarnings;
//------------------------------------------------------------------------------

class JGerber{
private:
  FILE_WRAPPER File;

  char*    Buffer;
  unsigned Length;
  unsigned Index;
  unsigned LineNumber;

  GerberLevel* CurrentLevel;
  GerberLevel* LastLevel;
  int          LevelsSize; // Buffer size

  void Add(GerberLevel* Level);

  // Tokens
  void   WhiteSpace   ();
  bool   GetInteger   (int*    Integer);
  bool   GetFloat     (double* Number);
  bool   GetCoordinate(double* Number, int Integer, int Decimal);
  bool   GetString    (char*   String);
  double Get_mm       (double  Number);

  // N Codes: Sequence numbers
  bool NCode();

  // G Codes: General functions
  bool GCode();

  // D Codes: Plot functions
  bool DCode();

  // M Codes: Miscellaneous functions
  bool MCode(bool* EndOfFile);

  // Parameters Variables and Structures:
  struct FORMAT{
    bool OmitTrailingZeroes;
    int  XInteger;
    int  XDecimal;
    int  YInteger;
    int  YDecimal;
  };
  FORMAT Format;
  bool   Incremental;
  double OffsetA, OffsetB;
  double ScaleA , ScaleB;

  GERBER_UNIT Units;

  GerberAperture** Apertures;

  // Macro structures
  struct MACRO_ITEM{
    GerberMacro* Macro;
    MACRO_ITEM*  Next;

    MACRO_ITEM();
   ~MACRO_ITEM();
  };
  MACRO_ITEM* Macros; //Macro Stack

  void         Add      (GerberMacro* Macro);
  GerberMacro* FindMacro(const char*  Name);

  bool StartOfLevel;

  // Directive Parameters
  bool AxisSelect     ();
  bool FormatStatement();
  bool MirrorImage    ();
  bool Mode           ();
  bool Offset         ();
  bool ScaleFactor    ();

  // Unknown Parameters
  bool IC();

  // Image Parameters
  bool ImageJustify ();
  bool ImageName    ();
  bool ImageOffset  ();
  bool ImagePolarity();
  bool ImageRotation();
  bool PlotFilm     ();

  // Aperture Parameters
  bool Add(GerberAperture* Aperture);

  bool ApertureCircle   (int Code);
  bool ApertureRectangle(int Code);
  bool ApertureObround  (int Code);
  bool AperturePolygon  (int Code);
  bool ApertureMacro    (int Code, const char* Name);

  bool ApertureDefinition();
  bool ApertureMacro     ();

  // Level-Specific Parameters
  bool Knockout     ();
  bool LevelName    ();
  bool LevelPolarity();
  bool StepAndRepeat();

  // Miscellaneous Parameters
  bool IncludeFile();

  // Attributes
  bool Attribute();

  bool Parameter(char Delimiter);

  // Root of the parser
  void SetBBox();

  bool GetGerber();

  void Initialise();
  void Cleanup   ();

public:
  JGerber();
 ~JGerber();

  bool  Negative; // Image Polarity
  char* Name;     // Image Name

  GerberLevel* Levels; // Linked list; Read this to get the levels, do not modify

  // Image bounding box
  double Left;
  double Bottom;
  double Right;
  double Top;

  void Clear();
  bool LoadGerber(const char* FileName);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

