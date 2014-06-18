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

#include "JGerber.h"
//------------------------------------------------------------------------------

bool Equal(const char* s1, const char* s2){
 int j;

 if(!s1 && !s2) return true;
 if(!s1) return false;
 if(!s2) return false;

 for(j = 0; s1[j] && s2[j]; j++){
  if(s1[j] != s2[j]) return false;
 }
 if(s1[j] != s2[j]) return false;
 return true;
}
//------------------------------------------------------------------------------

void JGerber::Initialise(){
 int j;

 Buffer = 0;
 Length = 0;
 Index  = 0;
 Name   = 0;

 Units        = guInches;
 StartOfLevel = false;
 Incremental  = false;
 Negative     = false;

 Format.OmitTrailingZeroes = false;
 Format.XInteger           = 6;
 Format.XDecimal           = 6;
 Format.YInteger           = 6;
 Format.YDecimal           = 6;

 Levels       = 0;
 LastLevel    = 0;
 CurrentLevel = 0;

 Left   =  1e3;
 Bottom =  1e3;
 Right  = -1e3;
 Top    = -1e3;

 OffsetA = OffsetB = 0.0;
 ScaleA  = ScaleB  = 1.0;

 Apertures = new GerberAperture*[1000];
 for(j = 0; j < 1000; j++){
  Apertures[j] = 0;
 };

 Macros = 0;
}
//------------------------------------------------------------------------------

void JGerber::Cleanup(){
 int j;

 if(Levels) delete Levels;

 for(j = 0; j < 1000; j++){
  if(Apertures[j]) delete Apertures[j];
 };
 delete[] Apertures;

 MACRO_ITEM* TempMacro;
 while(Macros){
  TempMacro = Macros;
  Macros    = Macros->Next;
  delete TempMacro;
 }

 if(Name) delete[] Name;
}
//------------------------------------------------------------------------------

JGerber::JGerber(){
 Initialise();
}
//------------------------------------------------------------------------------

JGerber::~JGerber(){
 Cleanup();
}
//------------------------------------------------------------------------------

JGerber::MACRO_ITEM::MACRO_ITEM(){
 Macro = 0;
 Next  = 0;
}
//------------------------------------------------------------------------------

JGerber::MACRO_ITEM::~MACRO_ITEM(){
 if(Macro) delete Macro;
}
//------------------------------------------------------------------------------

void JGerber::Add(GerberMacro* Macro){
 MACRO_ITEM* Temp;

 if(!Macro) return;

 Temp        = new MACRO_ITEM;
 Temp->Macro = Macro;
 Temp->Next  = Macros;
 Macros      = Temp;
}
//------------------------------------------------------------------------------

GerberMacro* JGerber::FindMacro(const char* Name){
 MACRO_ITEM* Temp;

 if(!Macros) return 0;

 // Perform a linear search: Fast enough for the given number of macros
 Temp = Macros;
 while(Temp){
  if(Equal(Temp->Macro->Name, Name)) return Temp->Macro;
  Temp = Temp->Next;
 }

 return 0;
}
//------------------------------------------------------------------------------

void JGerber::WhiteSpace(){
 while(Index < Length){
  switch(Buffer[Index]){
   case '\n':
    LineNumber++;
   case ' ' :
   case '\t':
   case '\r':
    Index++;
    break;

   default:
    return;
  }
 }
}
//------------------------------------------------------------------------------

bool JGerber::GetInteger(int* Integer){
 bool     Sign = false;
 unsigned i    = Index;

 WhiteSpace();

 *Integer = 0;

 if(Index < Length && Buffer[Index] == '-'){
  Sign = true;
  Index++;
 }

 while(Index < Length){
  if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
   *Integer *= 10;
   *Integer += Buffer[Index] - '0';
   Index++;
  }else{
   if(Sign) *Integer *= -1;
   return (Index > i);
  }
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::GetFloat(double* Number){
 int       Integer = 0;
 bool      Sign    = false;
 double    Scale   = 0.1;
 unsigned  i       = Index;

 WhiteSpace();

 if(Index < Length && Buffer[Index] == '-'){
  Sign = true;
  Index++;
 }

 while(Index < Length){
  if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
   Integer *= 10;
   Integer += Buffer[Index] - '0';
   Index++;
  }else{
   break;
  }
 }

 *Number = Integer;

 if(Index < Length && Buffer[Index] == '.'){
  Index++;
  while(Index < Length){
   if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
    *Number += (Buffer[Index] - '0') * Scale;
    Scale *= 0.1;
    Index++;
   }else{
    break;
   }
  }
 }

 if(Index < Length){
  if(Sign) *Number *= -1.0;
  return (Index > i);
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::GetCoordinate(double* Number, int Integer, int Decimal){
 int      j;
 int      n    = 0;
 bool     Sign = false;
 unsigned i    = Index;

 WhiteSpace();

 *Number = 0;

 if(Index < Length && Buffer[Index] == '-'){
  Sign = true;
  Index++;
 }

 while(Index < Length){
  if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
   *Number *= 10;
   *Number += Buffer[Index] - '0';
   Index++;
   n++;

  }else if(Buffer[Index] == '.'){
   Index = i;
   return GetFloat(Number);

  }else{
   if(Sign) *Number *= -1;
   if(Format.OmitTrailingZeroes){
    for(j = 0; j < (Integer + Decimal - n); j++){
     *Number *= 10;
    }
   }
   for(j = 0; j < Decimal; j++) *Number /= 10;
   return n;
  }
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::GetString(char* String){
 int      j;
 unsigned i = Index;

 WhiteSpace();

 j = 0;
 while(Index < Length){
  switch(Buffer[Index]){
   case 0:
    printf("Line %d - Error: Null in name not allowed\n", LineNumber);
    return false;

   case '*':
   case ',':
    String[j] = 0;
    return true;

   default:
    String[j++] = Buffer[Index++];
    break;
  }
  WhiteSpace();
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

double JGerber::Get_mm(double Number){
 if(Units == guInches) Number *= 25.4;
 return Number;
}
//------------------------------------------------------------------------------

bool JGerber::NCode(){
 unsigned i = Index;

 StartOfLevel = false;

 printf("Line %d - Error: N Code not implemented\n", LineNumber);

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::GCode(){
 int          Code;
 unsigned     i = Index;
 GerberLevel* Level;

 StartOfLevel = false;

 if(!GetInteger(&Code)) return false;

 if(!CurrentLevel && Code != 4){
  Level = new GerberLevel(CurrentLevel);
  Level->Units = Units;
  Add(Level);
 }

 switch(Code){
  case 0: // Move
   CurrentLevel->Exposure      = geOff;
   CurrentLevel->Interpolation = giLinear;
   return true;

  case 1: // Linear interpolation 1X scale
   CurrentLevel->Interpolation = giLinear;
   return true;

  case 2: // Clockwise circular
   CurrentLevel->Interpolation = giClockwiseCircular;
   return true;

  case 3: // Counterclockwise circular
   CurrentLevel->Interpolation = giCounterclockwiseCircular;
   return true;

  case 4: // Ignore block
   while(Index < Length && Buffer[Index] != '*') Index++;
   Index++;
   return (Index < Length);

  case 10: // Linear interpolation 10X scale
   CurrentLevel->Interpolation = giLinear10X;
   return true;

  case 11: // Linear interpolation 0.1X scale
   CurrentLevel->Interpolation = giLinear0_1X;
   return true;

  case 12: // Linear interpolation 0.01X scale
   CurrentLevel->Interpolation = giLinear0_01X;
   return true;

  case 36: // Turn on Outline Area Fill
   CurrentLevel->OutlineBegin();
   return true;

  case 37: // Turn off Outline Area Fill
   CurrentLevel->OutlineEnd();
   return true;

  case 54: // Tool prepare
   CurrentLevel->Exposure = geOff;
   return true;

  case 55: // Flash prepare
   CurrentLevel->Exposure = geOff;
   return true;

  case 70: // Specify inches
   CurrentLevel->Units = guInches;
   return true;

  case 71: // Specify millimeters
   CurrentLevel->Units = guMillimeters;
   return true;

  case 74: // Disable 360 deg circular interpolation
   CurrentLevel->Multiquadrant = false;
   return true;

  case 75: // Enable 360 deg circular interpolation
   CurrentLevel->Multiquadrant = true;
   return true;

  case 90: // Specify absolute format
   CurrentLevel->Incremental = false;
   return true;

  case 91: // Specify incremental format
   printf(
    "Line %d - Error: GCode -> Incremental not implemented\n",
    LineNumber
   );
   return true;

  default:
   printf("Line %d - Error: Unknown G Code: %d\n", LineNumber, Code);
   break;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::DCode(){
 int             Code;
 unsigned        i = Index;
 GerberLevel*    Level;
 GerberAperture* Aperture;

 if(!CurrentLevel){
  Level = new GerberLevel(CurrentLevel);
  Level->Units = Units;
  Add(Level);
 }

 StartOfLevel = false;

 if(!GetInteger(&Code)) return false;

 switch(Code){
  case 1: // Draw line, exposure on
   CurrentLevel->Exposure = geOn;
   return true;

  case 2: // Exposure off
   CurrentLevel->Exposure = geOff;
   return true;

  case 3: // Flash aperture
   CurrentLevel->Exposure = geFlash;
   return true;

  default: // Select aperture
   if(Code >= 1000){
    printf(
     "Line %d - Error: Aperture code out of range: D%d\n",
     LineNumber,
     Code
    );
    return false;
   }
   Aperture = Apertures[Code];
   if(!Aperture){
    printf("Line %d - Error: Aperture not defined: D%d\n", LineNumber, Code);
    return false;
   }
   CurrentLevel->ApertureSelect(Aperture);
   return true;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::MCode(bool* EndOfFile){
 int Code;

 StartOfLevel = false;

 if(!GetInteger(&Code)) return false;

 switch(Code){
  case 0: // Program stop
  case 1: // Optional stop
  case 2: // End of program
   if(CurrentLevel){
    CurrentLevel->Exposure = geOff;
    CurrentLevel->Do();
   }
   *EndOfFile = true;
   return true;

  default: // Select aperture
   printf("Line %d - Error: Unknown M Code: %d\n", LineNumber, Code);
   return false;
 }
}
//------------------------------------------------------------------------------

bool JGerber::Add(GerberAperture* Aperture){
 if(Aperture->Code >= 1000){
  printf(
   "Line %d - Error: Aperture code out of range: D%d\n",
   LineNumber,
   Aperture->Code
  );
  delete Aperture;
  return false;
 }
 if(Apertures[Aperture->Code]){
  printf(
   "Line %d - Error: Overloading of apertures not supported: D%d\n",
   LineNumber,
   Aperture->Code
  );
  delete Aperture;
  return false;
 }

 Apertures[Aperture->Code] = Aperture;
 return true;
}
//------------------------------------------------------------------------------

bool JGerber::ApertureCircle(int Code){
 double d, x, y;
 GerberAperture* Aperture;

 d =  0.0;
 x = -1.0;
 y = -1.0;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] != ',') return false;
 Index++;

 if(!GetFloat(&d)) return false;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] == 'X'){
  Index++;
  if(!GetFloat(&x)) return false;
  WhiteSpace();
  if(Index >= Length) return false;
  if(Buffer[Index] == 'X'){
   Index++;
   if(!GetFloat(&y)) return false;
  }
 }

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 Aperture = new GerberAperture;
 Aperture->Code = Code;
 Aperture->Circle(Get_mm(d));
 if(x > 0.0){
  if(y > 0.0){
   Aperture->HoleRectangle(Get_mm(x), Get_mm(y));
  }else{
   Aperture->HoleCircle(Get_mm(x));
  }
 }
 return Add(Aperture);
}
//------------------------------------------------------------------------------

bool JGerber::ApertureRectangle(int Code){
 double w, h, x, y;
 GerberAperture* Aperture;

 w = h =  0.0;
 x = y = -1.0;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] != ',') return false;
 Index++;

 if(!GetFloat(&w)) return false;

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != 'X') return false;
 Index++;
 if(!GetFloat(&h))        return false;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] == 'X'){
  Index++;
  if(!GetFloat(&x)) return false;
  WhiteSpace();
  if(Index >= Length) return false;
  if(Buffer[Index] == 'X'){
   Index++;
   if(!GetFloat(&y)) return false;
  }
 }

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 Aperture = new GerberAperture;
 Aperture->Code = Code;
 Aperture->Rectangle(Get_mm(w), Get_mm(h));
 if(x > 0.0){
  if(y > 0.0){
   Aperture->HoleRectangle(Get_mm(x), Get_mm(y));
  }else{
   Aperture->HoleCircle(Get_mm(x));
  }
 }
 return Add(Aperture);
}
//------------------------------------------------------------------------------

bool JGerber::ApertureObround(int Code){
 double w, h, x, y;
 GerberAperture* Aperture;

 w = h =  0.0;
 x = y = -1.0;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] != ',') return false;
 Index++;

 if(!GetFloat(&w)) return false;

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != 'X') return false;
 Index++;
 if(!GetFloat(&h))        return false;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] == 'X'){
  Index++;
  if(!GetFloat(&x)) return false;
  WhiteSpace();
  if(Index >= Length) return false;
  if(Buffer[Index] == 'X'){
   Index++;
   if(!GetFloat(&y)) return false;
  }
 }

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 Aperture = new GerberAperture;
 Aperture->Code = Code;
 Aperture->Obround(Get_mm(w), Get_mm(h));
 if(x > 0.0){
  if(y > 0.0){
   Aperture->HoleRectangle(Get_mm(x), Get_mm(y));
  }else{
   Aperture->HoleCircle(Get_mm(x));
  }
 }
 return Add(Aperture);
}
//------------------------------------------------------------------------------

bool JGerber::AperturePolygon(int Code){
 int    n;
 double w, a, x, y;
 GerberAperture* Aperture;

 n =  0;
 w =  0.0;
 a =  0.0;
 x = -1.0;
 y = -1.0;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] != ',') return false;
 Index++;

 if(!GetFloat(&w)) return false;

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != 'X') return false;
 Index++;
 if(!GetInteger(&n))      return false;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] == 'X'){
  Index++;
  if(!GetFloat(&a)) return false;
  WhiteSpace();
  if(Index >= Length) return false;
  if(Buffer[Index] == 'X'){
   Index++;
   if(!GetFloat(&x)) return false;
   WhiteSpace();
   if(Index >= Length) return false;
   if(Buffer[Index] == 'X'){
    Index++;
    if(!GetFloat(&y)) return false;
   }
  }
 }

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;


 Aperture = new GerberAperture;
 Aperture->Code = Code;
 Aperture->Polygon(Get_mm(w), n, a);
 if(x > 0.0){
  if(y > 0.0){
   Aperture->HoleRectangle(Get_mm(x), Get_mm(y));
  }else{
   Aperture->HoleCircle(Get_mm(x));
  }
 }
 return Add(Aperture);
}
//------------------------------------------------------------------------------

bool JGerber::ApertureMacro(int Code, const char* Name){
 int  j;
 bool b;

 GerberAperture* Aperture;
 GerberMacro*    Macro;
 double*         TempModifiers;

 // Variable length array; Null terminated
 double* Modifiers       = new double[16];
 int     ModifiersSize   = 16;
 int     ModifiersLength = 0;

 Macro = FindMacro(Name);
 if(!Macro){
  printf("Line %d - Error: Macro not defined: %s\n", LineNumber, Name);
  delete[] Modifiers;
  return false;
 }

 if(Index >= Length){
  delete[] Modifiers;
  return false;
 }

 WhiteSpace();

 if(Buffer[Index] != ','){
  Aperture       = new GerberAperture;
  Aperture->Code = Code;
  Aperture->UseMacro(Macro, Modifiers, 0);
  delete[] Modifiers;
  WhiteSpace();
  if(Index >= Length)      return false;
  if(Buffer[Index] != '*') return false;
  Index++;
  return Add(Aperture);
 }
 Index++;

 if(!GetFloat(Modifiers)){
  delete[] Modifiers;
  return false;
 }
 ModifiersLength++;

 WhiteSpace();

 while(Index < Length){
  if(Buffer[Index] == 'X'){
   Index++;

   if(ModifiersLength == ModifiersSize){
    ModifiersSize <<= 1;
    TempModifiers = new double[ModifiersSize];
    for(j = 0; j < ModifiersLength; j++){
     TempModifiers[j] = Modifiers[j];
    }
    delete[] Modifiers;
    Modifiers = TempModifiers;
   }

   if(!GetFloat(Modifiers+ModifiersLength)){
    delete[] Modifiers;
    return false;
   }
   ModifiersLength++;
  }else{
   break;
  }
  WhiteSpace();
 }

 WhiteSpace();

 b = false;
 if(Index < Length && Buffer[Index] == '*'){
  Index++;
  Aperture       = new GerberAperture;
  Aperture->Code = Code;
  Aperture->UseMacro(Macro, Modifiers, ModifiersLength);
  b = Add(Aperture);
 }

 delete[] Modifiers;

 return b;
}
//------------------------------------------------------------------------------

bool JGerber::ApertureDefinition(){
 int    Code;
 char   ApertureType[0x100];

 StartOfLevel = false;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] != 'D') return false;
 Index++;
 if(!GetInteger(&Code)) return false;

 if(!GetString(ApertureType)) return false;

 if(ApertureType[1]){
  return ApertureMacro(Code, ApertureType);
 }

 switch(ApertureType[0]){
  case 'C':
   return ApertureCircle(Code);

  case 'R':
   return ApertureRectangle(Code);

  case 'O':
   return ApertureObround(Code);

  case 'P':
   return AperturePolygon(Code);

  default:
   return ApertureMacro(Code, ApertureType);
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::ApertureMacro(){
 int          j;
 char         Name[0x100];
 GerberMacro* Macro;

 StartOfLevel = false;

 if(!GetString(Name)) return false;
 for(j = 0; Name[j]; j++);

 Macro = new GerberMacro;
 Macro->Name = new char[j+1];
 for(j = 0; Name[j]; j++){
  Macro->Name[j] = Name[j];
 }
 Macro->Name[j] = 0;

 WhiteSpace();

 if(Index >= Length) return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 WhiteSpace();

 j = Index;
 while(Index < Length){
  if(Buffer[Index] == '%'){
   Macro->LoadMacro(Buffer+j, Index-j, Units==guInches);
   Add(Macro);
   return true;
  }
  Index++;
  WhiteSpace();
 }

 delete Macro;
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::AxisSelect(){
 printf("Line %d - Warning: AxisSelect ignored\n", LineNumber);

 StartOfLevel = false;

 WhiteSpace();

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::FormatStatement(){
 StartOfLevel = false;

 WhiteSpace();

 while(Index < Length-1){
  switch(Buffer[Index]){
   case 'L':
    Format.OmitTrailingZeroes = false;
    break;

   case 'T':
    Format.OmitTrailingZeroes = true;
    break;

   case 'A':
    Incremental = false;
    break;

   case 'I':
    Incremental = true;
    break;

   case 'N':
   case 'G':
   case 'D':
   case 'M':
    Index++;
    break;

   case 'X':
    Index++;
    Format.XInteger = Buffer[Index] - '0';
    Index++;
    Format.XDecimal = Buffer[Index] - '0';
    break;

   case 'Y':
    Index++;
    Format.YInteger = Buffer[Index] - '0';
    Index++;
    Format.YDecimal = Buffer[Index] - '0';
    break;

   case '*':
    Index++;
    return true;

   default:
    printf(
     "Line %d - Error: Unrecognised FS modifier: %c\n",
     LineNumber,
     Buffer[Index]
    );
    return false;
  }
  Index++;
  WhiteSpace();
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::IncludeFile(){
 printf("Line %d - Error: IncludeFile not implimented\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::IC(){
 printf("Line %d - Warning: IC Paremeter ignored\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::ImageJustify(){
 printf("Line %d - Warning: ImageJustify ignored\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::ImageName(){
 int  j;
 char name[0x100];

 StartOfLevel = false;

 if(!GetString(name)) return false;

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 if(Name) delete[] Name;
 for(j = 0; name[j]; j++);

 Name = new char[j+1];
 for(j = 0; name[j]; j++){
  Name[j] = name[j];
 }
 Name[j] = 0;

 return true;
}
//------------------------------------------------------------------------------

bool JGerber::ImageOffset(){
 printf("Line %d - Error: ImageOffset not implimented\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::ImagePolarity(){

 StartOfLevel = false;

 WhiteSpace();

 if(Index >= Length-2) return false;
       if(Buffer[Index  ] == 'P' &&
          Buffer[Index+1] == 'O' &&
          Buffer[Index+2] == 'S' ){
  Negative = false;

 }else if(Buffer[Index  ] == 'N' &&
          Buffer[Index+1] == 'E' &&
          Buffer[Index+2] == 'G' ){
  Negative = true;

 }else{
  printf("Line %d - Error: Unknown Image Polarity\n", LineNumber);
  return false;
 }
 Index += 3;

 WhiteSpace();

 if(Index >= Length) return false;

 if(Buffer[Index] != '*') return false;
 Index += 1;
 return true;
}
//------------------------------------------------------------------------------

bool JGerber::ImageRotation(){
 printf("Line %d - Error: ImageRotation not implimented\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

void JGerber::Add(GerberLevel* Level){
 if(CurrentLevel){
  CurrentLevel->Exposure = geOff;
  CurrentLevel->Do();
 }

 if(Levels){
  LastLevel->Next = Level;
 }else{
  Levels = Level;
 }

 LastLevel    = Level;
 CurrentLevel = Level;
}
//------------------------------------------------------------------------------

bool JGerber::Knockout(){
 printf("Line %d - Error: Knockout not implimented\n", LineNumber);
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::LevelName(){
 char         Name[0x100];
 GerberLevel* Level;

 if(!GetString(Name))     return false;

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 if(!StartOfLevel){
  Level = new GerberLevel(CurrentLevel);
  Level->Units = Units;
  Level->SetName(Name);
  Add(Level);

 }else{
  CurrentLevel->SetName(Name);
 }
 StartOfLevel = true;

 return true;
}
//------------------------------------------------------------------------------

bool JGerber::LevelPolarity(){
 GerberLevel* Level;

 if(!StartOfLevel){
  Level = new GerberLevel(CurrentLevel);
  Level->Units = Units;
  Add(Level);
 }
 StartOfLevel = true;

 WhiteSpace();

 if(Index >= Length) return false;
 switch(Buffer[Index]){
  case 'C':
   CurrentLevel->Negative = true;
   break;

  case 'D':
   CurrentLevel->Negative = false;
   break;

  default:
   printf(
    "Line %d - Error: Unknown level polarity: %c\n",
    LineNumber,
    Buffer[Index]
   );
   return false;
 }
 Index++;

 WhiteSpace();

 if(Index >= Length)      return false;
 if(Buffer[Index] != '*') return false;
 Index++;

 return true;
}
//------------------------------------------------------------------------------

bool JGerber::MirrorImage(){
 printf("Line %d - Error: MirrorImage not implimented\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::Mode(){
 StartOfLevel = false;

 WhiteSpace();

 while(Index < Length-1){
  if      (Buffer[Index] == 'I' && Buffer[Index+1] == 'N'){
   Units = guInches;
   Index += 2;

  }else if(Buffer[Index] == 'M' && Buffer[Index+1] == 'M'){
   Units = guMillimeters;
   Index += 2;

  }else if(Buffer[Index] == '*'){
   Index++;
   return true;

  }else{
   printf(
    "Line %d - Error: Unrecognised MO modifier: %c\n",
    LineNumber,
    Buffer[Index]
   );
   return false;
  }
  WhiteSpace();
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::Offset(){
 double d;

 StartOfLevel = false;

 WhiteSpace();

 while(Index < Length){
  switch(Buffer[Index]){
   case 'A':
    Index++;
    if(!GetCoordinate(&d, 5, 5)) return false;
    OffsetA = d;
    break;

   case 'B':
    Index++;
    if(!GetCoordinate(&d, 5, 5)) return false;
    OffsetB = d;
    break;

   case '*':
    Index++;
    if(OffsetA != 0.0 || OffsetB != 0.0){
     printf("Line %d - Warning: Offsets ignored\n", LineNumber);
    }
    return true;

   default:
    printf(
     "Line %d - Error: Unrecognised Offset Modifier: %c\n",
     LineNumber,
     Buffer[Index]
    );
    return false;
  }
  WhiteSpace();
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::PlotFilm(){
 printf("Line %d - Error: PlotFilm not implimented\n", LineNumber);

 StartOfLevel = false;

 while(Index < Length){
  if(Buffer[Index] == '%') return true;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::ScaleFactor(){
 double d;

 StartOfLevel = false;

 WhiteSpace();

 while(Index < Length){
  switch(Buffer[Index]){
   case 'A':
    Index++;
    if(!GetFloat(&d)) return false;
    ScaleA = d;
    break;

   case 'B':
    Index++;
    if(!GetFloat(&d)) return false;
    ScaleB = d;
    break;

   case '*':
    Index++;
    if(ScaleB != 1.0 || ScaleB != 1.0){
     printf("Line %d - Warning: Scale Factor ignored\n", LineNumber);
    }
    return true;

   default:
    printf(
     "Line %d - Error: Unrecognised Scale Factor Modifier: %c\n",
     LineNumber,
     Buffer[Index]
    );
    return false;
  }
  WhiteSpace();
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::StepAndRepeat(){
 int          x, y;
 double       i, j;
 GerberLevel* Level;

 if(!StartOfLevel){
  Level = new GerberLevel(CurrentLevel);
  Level->Units = Units;
  Add(Level);
 }
 StartOfLevel = true;

 x = y = 1;
 i = j = 0.0;

 WhiteSpace();

 while(Index < Length){
  switch(Buffer[Index]){
   case 'X':
    Index++;
    if(!GetInteger(&x)) return false;
    break;

   case 'Y':
    Index++;
    if(!GetInteger(&y)) return false;
    break;

   case 'I':
    Index++;
    if(!GetFloat(&i)) return false;
    break;

   case 'J':
    Index++;
    if(!GetFloat(&j)) return false;
    break;

   case '*':
    Index++;
    CurrentLevel->CountX = x;
    CurrentLevel->CountY = y;
    CurrentLevel->StepX  = Get_mm(i);
    CurrentLevel->StepY  = Get_mm(j);
    return true;

   default:
    printf(
     "Line %d - Error: Unrecognised Step-and-Repeat Modifier: %c\n",
     LineNumber,
     Buffer[Index]
    );
    return false;
  }
  WhiteSpace();
 }

 return false;
}
//------------------------------------------------------------------------------

bool JGerber::Attribute(){
 printf("Line %d - Warning: Attribute ignored\n", LineNumber);

 while(Index < Length){
  if(Buffer[Index] == '*'){
   Index++;
   return true;
  }
  Index++;
 }
 return false;
}
//------------------------------------------------------------------------------

bool JGerber::Paramater(char Delimiter){
 unsigned i = Index;

 WhiteSpace();

 while(Index < Length-1){
  if      (Buffer[Index] == 'A' && Buffer[Index+1] == 'D'){
   Index += 2;
   if(!ApertureDefinition()) return false;

  }else if(Buffer[Index] == 'A' && Buffer[Index+1] == 'M'){
   Index += 2;
   if(!ApertureMacro()) return false;

  }else if(Buffer[Index] == 'A' && Buffer[Index+1] == 'S'){
   Index += 2;
   if(!AxisSelect()) return false;

  }else if(Buffer[Index] == 'A' && Buffer[Index+1] == 'V'){
   Index += 2;
   if(!Attribute()) return false;

  }else if(Buffer[Index] == 'D' && Buffer[Index+1] == 'R'){
   Index += 2;
   if(!Attribute()) return false;

  }else if(Buffer[Index] == 'F' && Buffer[Index+1] == 'S'){
   Index += 2;
   if(!FormatStatement()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'C'){
   Index += 2;
   if(!IC()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'F'){
   Index += 2;
   if(!IncludeFile()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'J'){
   Index += 2;
   if(!ImageJustify()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'N'){
   Index += 2;
   if(!ImageName()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'O'){
   Index += 2;
   if(!ImageOffset()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'P'){
   Index += 2;
   if(!ImagePolarity()) return false;

  }else if(Buffer[Index] == 'I' && Buffer[Index+1] == 'R'){
   Index += 2;
   if(!ImageRotation()) return false;

  }else if(Buffer[Index] == 'K' && Buffer[Index+1] == 'O'){
   Index += 2;
   if(!Knockout()) return false;

  }else if(Buffer[Index] == 'L' && Buffer[Index+1] == 'N'){
   Index += 2;
   if(!LevelName()) return false;

  }else if(Buffer[Index] == 'L' && Buffer[Index+1] == 'P'){
   Index += 2;
   if(!LevelPolarity()) return false;

  }else if(Buffer[Index] == 'M' && Buffer[Index+1] == 'I'){
   Index += 2;
   if(!MirrorImage()) return false;

  }else if(Buffer[Index] == 'M' && Buffer[Index+1] == 'O'){
   Index += 2;
   if(!Mode()) return false;

  }else if(Buffer[Index] == 'O' && Buffer[Index+1] == 'F'){
   Index += 2;
   if(!Offset()) return false;

  }else if(Buffer[Index] == 'P' && Buffer[Index+1] == 'F'){
   Index += 2;
   if(!PlotFilm()) return false;

  }else if(Buffer[Index] == 'S' && Buffer[Index+1] == 'F'){
   Index += 2;
   if(!ScaleFactor()) return false;

  }else if(Buffer[Index] == 'S' && Buffer[Index+1] == 'R'){
   Index += 2;
   if(!StepAndRepeat()) return false;

  }else if(Buffer[Index] == 'T' && Buffer[Index+1] == 'A'){
   Index += 2;
   if(!Attribute()) return false;

  }else if(Buffer[Index] == 'T' && Buffer[Index+1] == 'D'){
   Index += 2;
   if(!Attribute()) return false;

  }else if(Buffer[Index] == 'T' && Buffer[Index+1] == 'F'){
   Index += 2;
   if(!Attribute()) return false;

  }else if(Buffer[Index] == Delimiter){
   Index++;
   return true;

  }else{
   printf(
    "Line %d - Error: Unknown parameter: %c%c\n",
    LineNumber,
    Buffer[Index],
    Buffer[Index+1]
   );
   return false;
  }
  WhiteSpace();
 }

 printf("Line %d - Error: Parameter Block without end-delimiter\n", LineNumber);

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

void JGerber::SetBBox(){
 GerberLevel* Temp = Levels;

 double l, b, r, t, w, h;

 if(!Temp) Left = Bottom = Right = Top = 0.0;

 while(Temp){
  l = Temp->Left;
  b = Temp->Bottom;
  r = Temp->Right;
  t = Temp->Top;

  w = r-l;
  h = t-b;

  if(Temp->CountX > 1){
   r = l + Temp->StepX*(Temp->CountX-1) + w;
  }

  if(Temp->CountY > 1){
   t = b + Temp->StepY*(Temp->CountY-1) + h;
  }

  if(Left   > l) Left   = l;
  if(Bottom > b) Bottom = b;
  if(Right  < r) Right  = r;
  if(Top    < t) Top    = t;

  Temp = Temp->Next;
 }
}
//------------------------------------------------------------------------------

bool JGerber::GetGerber(){
 bool   EndOfFile;
 double d;

 WhiteSpace();

 while(Index < Length){
  switch(Buffer[Index]){
   case 'N':
    Index++;
    if(!NCode()) return false;
    break;

   case 'G':
    Index++;
    if(!GCode()) return false;
    break;

   case 'D':
    Index++;
    if(!DCode()) return false;
    break;

   case 'M':
    Index++;
    if(!MCode(&EndOfFile)) return false;
    if(EndOfFile){
     SetBBox();
     return true;
    }
    break;

   case 'X':
    Index++;
    if(!GetCoordinate(&d, Format.XInteger, Format.XDecimal)) return false;
    if(!CurrentLevel){
     printf("Line %d - Error: No level defined\n", LineNumber);
     return false;
    }
    CurrentLevel->X = d;
    break;

   case 'Y':
    Index++;
    if(!GetCoordinate(&d, Format.YInteger, Format.YDecimal)) return false;
    if(!CurrentLevel){
     printf("Line %d - Error: No level defined\n", LineNumber);
     return false;
    }
    CurrentLevel->Y = d;
    break;

   case 'I':
    Index++;
    if(!GetCoordinate(&d, Format.XInteger, Format.XDecimal)) return false;
    if(!CurrentLevel){
     printf("Line %d - Error: No level defined\n", LineNumber);
     return false;
    }
    CurrentLevel->I = d;
    break;

   case 'J':
    Index++;
    if(!GetCoordinate(&d, Format.YInteger, Format.YDecimal)) return false;
    if(!CurrentLevel){
     printf("Line %d - Error: No level defined\n", LineNumber);
     return false;
    }
    CurrentLevel->J = d;
    break;

   case '*':
    Index++;
    if(!CurrentLevel){
     printf("Line %d - Error: No level defined\n", LineNumber);
     return false;
    }
    CurrentLevel->Do();
    break;

   case ' ' :
   case '\t':
   case '\r':
   case '\n':
    Index++;
    break;

   default:
    Index++;
    if(!Paramater(Buffer[Index-1])) return false;
    break;

  }
  WhiteSpace();
 }

 printf("Line %d - Error: No end-of-file code\n", LineNumber);
 return false;
}
//------------------------------------------------------------------------------

void JGerber::Clear(){
 Cleanup   ();
 Initialise();
}
//------------------------------------------------------------------------------

bool JGerber::LoadGerber(const char* FileName){
 bool b;

 StartOfLevel = false;

 File.SetFilename(FileName);
 if(File.Open(JFile::Read)){
  Length     = File.GetSize();
  Buffer     = new char[Length];
  Index      = 0;
  LineNumber = 1;
  File.ReadBuffer(Buffer, Length, &b);
  File.Close();
  b = GetGerber();
  delete[] Buffer;
  return b;

 }else{
  File.ShowLastError();
 }

 return false;
}
//------------------------------------------------------------------------------
