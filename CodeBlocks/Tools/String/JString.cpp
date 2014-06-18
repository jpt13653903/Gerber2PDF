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

#include "JString.h"
//------------------------------------------------------------------------------

JString::JString(){
 Memory    = 4;
 String    = new char[Memory];
 String[0] = 0;
 Length    = 0;
}
//------------------------------------------------------------------------------

JString::~JString(){
 delete[] String;
}
//------------------------------------------------------------------------------

void JString::Expand(int Length){
 int   j;
 int   OldMemory;
 char* OldString;

 OldMemory = Memory;
 while(Memory <= Length) Memory <<= 1;

 if(Memory == OldMemory) return;

 OldString = String;
 String = new char[Memory];

 for(j = 0; OldString[j]; j++){
  String[j] = OldString[j];
 }
 String[j] = 0;

 delete[] OldString;
}
//------------------------------------------------------------------------------

void JString::Contract(int Length){
 int   j;
 int   OldMemory;
 char* OldString;

 if(Length < 2) Length = 2;

 OldMemory = Memory;
 Memory    = 4;
 while(Memory <= Length) Memory <<= 1;

 if(Memory == OldMemory) return;

 OldString = String;
 String = new char[Memory];

 for(j = 0; (OldString[j]) && (j < Length); j++){
  String[j] = OldString[j];
 }
 String[j]       = 0;
 JString::Length = j;

 delete[] OldString;
}
//------------------------------------------------------------------------------

int JString::GetLength(){
 return Length;
}
//------------------------------------------------------------------------------

void JString::Set(const char* String){
 int j, tLength;

 tLength = 0;
 while(String[tLength]) tLength++;

 if(tLength >= Memory){
  Expand(tLength);
 }else if((!tLength) || (Memory / tLength >= 0x10)){
  Contract(tLength);
 }

 Length = tLength;

 for(j = 0; j < Length; j++){
  JString::String[j] = String[j];
 }
 JString::String[j] = 0;
}
//------------------------------------------------------------------------------

void JString::Set(char c){
 Contract(1);
 String[0] = c;
 String[1] = 0;
 Length    = 1;
}
//------------------------------------------------------------------------------

void JString::Set(int i){
 bool Negative = false;

 if(i){
  Set("");

  if(i < 0){
   i *= -1;
   Negative = true;
  }

  while(i){ // To append is faster that to prefix
   Append((char)((i % 10) + '0'));
   i /= 10;
  }

  if(Negative) Append('-');

  Reverse();

 }else{
  Set("0");
 }
}
//------------------------------------------------------------------------------

void JString::Set(unsigned i, unsigned Places){ // Hexadecimal
 bool Negative = false;
 char c;

 Set("");

 while(i){ // To append is faster that to prefix
  c = (char)(i & 0x0F);
  if(c <= 9){
   c += '0';
  }else{
   c += 'A' - 10;
  }
  Append(c);
  Places--;
  i >>= 4;
 }

 while(Places > 0){
  Append("0");
  Places--;
 }

 if(Negative) Append('-');

 Reverse();
}
//------------------------------------------------------------------------------

static bool Equal(double d1, double d2){
 unsigned char* s1 = (unsigned char*)(&d1);
 unsigned char* s2 = (unsigned char*)(&d2);

 for(int j = 0; j < 10; j++){
  if(s1[j] != s2[j]) return false;
 }
 return true;
}
//------------------------------------------------------------------------------

static int mod10(double f){
 if(f < 10e16){
  return round(f - 10.0 * floor(f / 10.0));
 }else{
  return 0;
 }
}
//------------------------------------------------------------------------------

void JString::Set(double d, unsigned Places, bool Fill){
 double  i, f; // integer and fraction
 double  scale;
 JString s;

 if(d == 1.0/0.0){
  Set("INF");
  return;
 }else if(d == -1.0/0.0){
  Set("-INF");
  return;
 }else if(Equal(d, 0.0/0.0)){
  Set("NAN");
  return;
 }

 if(d < 0.0){
  Set('-');
  d *= -1.0;
 }else{
  Set("");
 }

 i     = d;
 scale = pow(10.0, Places);
 while(i > 1.0){
  scale /= 10.0;
  i     /= 10.0;
  Places--;
 }

 i = floor(d);
 f = round((d - i) * scale);
 while(f >= scale){
  i++;
  f = round(f - scale);
 }

 if(i == 0.0) Append('0');
 while(i >= 1.0){
  s.Append((char)(mod10(i) + '0'));
  i = floor(i / 10.0);
 }
 s.Reverse();
 Append   (s.String);
 s.Set    ("");

 bool b = false;
 if(f > 0.0){
  Append('.');
  while(Places){
   if(b || (mod10(f) > 0) || Fill){
    s.Append((char)(mod10(f) + '0'));
    b = true;
   }
   f = floor(f / 10.0);
   Places--;
  }

 }else if(Fill){
  Append('.');
  while(Places){
   s.Append('0');
   Places--;
  }
 }

 s.Reverse();

 Append(s.String);
}
//------------------------------------------------------------------------------

void JString::Append(const char* String){
 int j;
 int NewLength;

 NewLength = 0;
 while(String[NewLength]) NewLength++;
 if(!NewLength) return;

 NewLength += Length;

 if(NewLength >= Memory){
  Expand(NewLength);
 }

 for(j = Length; j < NewLength; j++){
  JString::String[j] = String[j-Length];
 }
 JString::String[j] = 0;

 Length = NewLength;
}
//------------------------------------------------------------------------------

void JString::Append(char c){
 if(Length + 1 >= Memory){
  Expand(Length + 1);
 }

 String[Length++] = c;
 String[Length  ] = 0;
}
//------------------------------------------------------------------------------

void JString::Append(int i){
 JString s;
 s.Set(i);
 Append(s.String);
}
//------------------------------------------------------------------------------

void JString::Append(unsigned i, unsigned Places){
 JString s;
 s.Set(i, Places);
 Append(s.String);
}
//------------------------------------------------------------------------------

void JString::Append(double d, unsigned Places, bool Fill){
 JString s;
 s.Set(d, Places, Fill);
 Append(s.String);
}
//------------------------------------------------------------------------------

void JString::Prefix(const char* String){
 int j;
 int AddLength;
 int NewLength;

 AddLength = 0;
 while(String[AddLength]) AddLength++;
 if(!AddLength) return;

 NewLength = AddLength + Length;

 if(NewLength >= Memory){
  Expand(NewLength);
 }

 for(j = Length; j >= 0; j--){
  JString::String[j+AddLength] = JString::String[j];
 }

 for(j = 0; j < AddLength; j++){
  JString::String[j] = String[j];
 }

 Length = NewLength;
}
//------------------------------------------------------------------------------

void JString::Prefix(char c){
 int j;

 if(Length + 1 >= Memory){
  Expand(Length + 1);
 }

 Length++;
 for(j = Length; j > 0; j--){
  String[j] = String[j-1];
 }
 String[0] = c;
}
//------------------------------------------------------------------------------

void JString::Prefix(int i){
 JString s;
 s.Set(i);
 Prefix(s.String);
}
//------------------------------------------------------------------------------

void JString::Prefix(unsigned i, unsigned Places){
 JString s;
 s.Set(i, Places);
 Prefix(s.String);
}
//------------------------------------------------------------------------------

void JString::Prefix(double d, unsigned Places, bool Fill){
 JString s;
 s.Set(d, Places, Fill);
 Prefix(s.String);
}
//------------------------------------------------------------------------------

void JString::Insert(const char* String, int Index){
 int j;
 int AddLength;
 int NewLength;

 if(Index < 0) return;
 if(Index > Length) return;

 AddLength = 0;
 while(String[AddLength]) AddLength++;
 if(!AddLength) return;

 NewLength = AddLength + Length;

 if(NewLength >= Memory){
  Expand(NewLength);
 }

 for(j = Length; j >= Index; j--){
  JString::String[j+AddLength] = JString::String[j];
 }

 for(j = 0; j < AddLength; j++){
  JString::String[j+Index] = String[j];
 }

 Length = NewLength;
}
//------------------------------------------------------------------------------

void JString::Insert(char c, int Index){
 int j;

 if(Index < 0) return;
 if(Index > Length) return;

 if(Length + 1 >= Memory){
  Expand(Length + 1);
 }

 Length++;
 for(j = Length; j > Index; j--){
  String[j] = String[j-1];
 }
 String[j] = c;
}
//------------------------------------------------------------------------------

void JString::Insert(int i, int Index){
 JString s;
 s.Set(i);
 Insert(s.String, Index);
}
//------------------------------------------------------------------------------

void JString::Insert(unsigned i, unsigned Places, int Index){
 JString s;
 s.Set(i, Places);
 Insert(s.String, Index);
}
//------------------------------------------------------------------------------

void JString::Insert(double d, unsigned Places, int Index){
 JString s;
 s.Set(d, Places);
 Insert(s.String, Index);
}
//------------------------------------------------------------------------------

void JString::Remove(int i){
 int j;

 if(i < 0) return;
 if(i >= Length) return;

 Length--;
 for(j = i; j < Length; j++){
  String[j] = String[j+1];
 }
 String[j] = 0;
}
//------------------------------------------------------------------------------

// Returns 0 when strings are equal
int JString::Compare(const char* String){
 int j;

 for(j = 0; JString::String[j] && String[j]; j++){
  if(JString::String[j] > String[j]) return  1;
  if(JString::String[j] < String[j]) return -1;
 }

 if(JString::String[j]) return  1;
 if(         String[j]) return -1;

 return 0;
}
//------------------------------------------------------------------------------

void JString::Reverse(){
 int  j, l;
 char c;

 l = Length/2;

 for(j = 0; j < l; j++){
  c = String[j];
  String[j] = String[Length-j-1];
  String[Length-j-1] = c;
 }
}
//------------------------------------------------------------------------------
