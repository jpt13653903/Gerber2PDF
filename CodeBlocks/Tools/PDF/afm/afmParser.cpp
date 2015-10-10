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

#include "afmParser.h"
//------------------------------------------------------------------------------

afmParser::afmParser(){
 Buffer = 0;
 Index  = 0;
 Length = 0;
}
//------------------------------------------------------------------------------

afmParser::~afmParser(){
 if(Buffer) delete[] Buffer;
}
//------------------------------------------------------------------------------

bool afmParser::Open(const char* FileName){
 bool b;

 File.SetFilename(FileName);
 if(File.Open(JFile::Read)){
  Index  = 0;
  Length = File.GetSize();
  Buffer = new char[Length];
  File.ReadBuffer(Buffer, Length, &b);
  File.Close();
  return b;

 }else{
  File.ShowLastError();
 }

 return false;
}
//------------------------------------------------------------------------------

void afmParser::Open_Buffer(const char* Buffer){
 int j;

 Index  = 0;
 for(Length = 0; Buffer[Length]; Length++);

 this->Buffer = new char[Length];
 for(j = 0; j < Length; j++) this->Buffer[j] = Buffer[j];
}
//------------------------------------------------------------------------------

void afmParser::Close(){
 if(Buffer) delete[] Buffer;
 Buffer = 0;
 Index  = 0;
 Length = 0;
}
//------------------------------------------------------------------------------

int afmParser::Compare(const char* s1, const char* s2){
 int j;

 for(j = 0; s1[j] && s2[j]; j++){
  if(s1[j] < s2[j]) return -1;
  if(s1[j] > s2[j]) return  1;
 }

 if(s1[j]) return  1;
 if(s2[j]) return -1;

 return 0;
}
//------------------------------------------------------------------------------

bool afmParser::GetEndOfLine(){
 GetWhiteSpace();

 if(Index < Length && Buffer[Index] == '\n'){
  Index++;
  return true;
 }

 if(Index < Length-1 && Buffer[Index] == '\r' && Buffer[Index+1] == '\n'){
  Index += 2;
  return true;
 }

 return false;
}
//------------------------------------------------------------------------------

void afmParser::GetWhiteSpace(){
 while(Index < Length){
  if(
   Buffer[Index] != ' ' &&
   Buffer[Index] != '\t'
  ) return;
  Index++;
 }
}
//------------------------------------------------------------------------------

char* afmParser::GetLine(){
 unsigned j    = 0;
 unsigned f    = Index;
 char*    Line = 0;

 while(f < Length && Buffer[f] != '\r' && Buffer[f] != '\n') f++;

 Line = new char[f - Index + 1];
 while(Index < f){
  Line[j++] = Buffer[Index++];
 }
 Line[j] = 0;

 GetEndOfLine();

 return Line;
}
//------------------------------------------------------------------------------

bool afmParser::FindEnd(char* Key){
 int  j;
 char String[0x100];

 Key[0] = 'E';
 Key[1] = 'n';
 Key[2] = 'd';

 for(j = 5; Key[j]; j++){
  Key[j-2] = Key[j];
 }
 Key[j-2] = 0;

 while(GetKey(String)){
  if(!Compare(String, Key)){
   GetEndOfLine();
   return true;
  }
  while(Index < Length && Buffer[Index] != '\n') Index++;
  Index++;
 }

 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetKey(char* Key){
 int      j = 0;
 unsigned i = Index;

 while(GetEndOfLine());

 while(Index < Length){
  switch(Buffer[Index]){
   case ' ':
   case '\t':
   case '\r':
   case '\n':
    Key[j] = 0;
    return (j > 0);

   default:
    break;
  }
  Key[j++] = Buffer[Index];
  Index++;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetString(char* String){
 int      j = 0;
 unsigned i = Index;

 GetWhiteSpace();

 while(Index < Length){
  switch(Buffer[Index]){
   case '\r':
   case '\n':
    String[j] = 0;
    return (j > 0);

   default:
    break;
  }
  String[j++] = Buffer[Index];
  Index++;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetName(char* Name){
 int      j = 0;
 unsigned i = Index;

 GetWhiteSpace();

 while(Index < Length){
  switch(Buffer[Index]){
   case ' ':
   case '\t':
   case '\r':
   case '\n':
    Name[j] = 0;
    return (j > 0);

   default:
    break;
  }
  Name[j++] = Buffer[Index];
  Index++;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetNumber(double* Number){
 bool     Negative = false;
 double   Integer  = 0.0;
 double   Fraction = 0.0;
 double   Scale    = 0.1;
 unsigned i        = Index;

 GetWhiteSpace();

 if(Index < Length && Buffer[Index] == '-'){
  Index++;
  Negative = true;
 }

 while(Index < Length){
  if(Buffer[Index] == '.'){
   Index++;
   while(Index < Length){
    if(Buffer[Index] < '0' || Buffer[Index] > '9'){
     if(Index == i+1){
      Index = i;
      return false;
     }
     *Number = Integer + Fraction;
     if(Negative) *Number *= -1.0;
     return true;
    }
    Fraction += (Buffer[Index] - '0') * Scale;
    Scale    /= 10.0;
    Index++;
   }
  }

  if(Buffer[Index] < '0' || Buffer[Index] > '9'){
   if(Index == i) return false;
   *Number = Integer;
   if(Negative) *Number *= -1.0;
   return true;
  }
  Integer *= 10.0;
  Integer += (Buffer[Index] - '0');
  Index++;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetHexInteger(unsigned* Integer){
 unsigned t = 0;
 unsigned i = Index;

 GetWhiteSpace();

 if(Index > Length) return false;
 if(Buffer[Index] != '<') return false;
 Index++;

 while(Index < Length){
  if(Buffer[Index] >= '0' && Buffer[Index] <= '9'){
   t *= 0x10;
   t += (Buffer[Index] - '0');
   Index++;

  }else if(Buffer[Index] >= 'a' && Buffer[Index] <= 'f'){
   t *= 0x10;
   t += (Buffer[Index] - 'a' + 10);
   Index++;

  }else if(Buffer[Index] >= 'A' && Buffer[Index] <= 'F'){
   t *= 0x10;
   t += (Buffer[Index] - 'A' + 10);
   Index++;

  }else{
   if(Index == i) return false;
   if(Buffer[Index] != '>'){
    Index = i;
    return false;
   }
   Index++;
   *Integer = t;
   return true;
  }
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetInteger(int* Integer){
 bool     Negative = false;
 int      t        = 0;
 unsigned i        = Index;

 GetWhiteSpace();

 if(Index < Length && Buffer[Index] == '-'){
  Index++;
  Negative = true;
 }

 if(Buffer[Index] == '<'){
  unsigned u;
  if(!GetHexInteger(&u)) return false;
  *Integer = u;
  return true;
 }

 while(Index < Length){
  if(Buffer[Index] < '0' || Buffer[Index] > '9'){
   if(Index == i) return false;
   *Integer = t;
   if(Negative) *Integer *= -1.0;
   return true;
  }
  t *= 10;
  t += (Buffer[Index] - '0');
  Index++;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::GetBoolean(bool* Boolean){
 unsigned i = Index;

 GetWhiteSpace();

 if(Index < Length-3       &&
    Buffer[Index  ] == 't' &&
    Buffer[Index+1] == 'r' &&
    Buffer[Index+2] == 'u' &&
    Buffer[Index+3] == 'e' ){
  *Boolean = true;
  return true;
 }

 if(Index < Length-4       &&
    Buffer[Index  ] == 'f' &&
    Buffer[Index+1] == 'a' &&
    Buffer[Index+2] == 'l' &&
    Buffer[Index+3] == 's' &&
    Buffer[Index+4] == 'e' ){
  *Boolean = false;
  return true;
 }

 Index = i;
 return false;
}
//------------------------------------------------------------------------------

bool afmParser::Ignore(char* Key){
 char c;

 c = Key[5];
 Key[5] = 0;
 if(!Compare(Key, "Start")){
  Key[5] = c;
  if(!FindEnd(Key)) return false;
 }else{
  while(Index < Length && Buffer[Index] != '\n') Index++;
  Index++;
 }

 return true;
}
//------------------------------------------------------------------------------

bool afmParser::IgnoreCharSection(){
 while(Index < Length){
  if(Buffer[Index] == ';'){
   Index++;
   break;

  }else if(Buffer[Index] == '\n'){
   break;
  }
  Index++;
 }

 if(Index < Length) return true;

 return false;
}
//------------------------------------------------------------------------------
