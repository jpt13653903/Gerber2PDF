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

#include "pdfString.h"
//------------------------------------------------------------------------------

pdfString::pdfString(){
 Value = 0;
}
//------------------------------------------------------------------------------

pdfString::~pdfString(){
 if(Value) delete[] Value;
}
//------------------------------------------------------------------------------

void pdfString::Set(const char* String){
 int j;
 for(j = 0; String[j]; j++);
 Set(String, j);
}
//------------------------------------------------------------------------------

void pdfString::Set(const char* String, unsigned Length){
 unsigned      j, q;
 unsigned char c;

 j = q = 0;
 while(j < Length){
  c = String[j];
  q++;
  if     (c == '(' || c == ')' || c == '\\') q++;
  else if(c  < ' ' || c  > '~') q += 3; // 0x20 and 0x7E
  j++;
 }

 if(Value) delete[] Value;
 Value = new char[q+1];

 j = q = 0;
 while(j < Length){
  c = String[j];
  if(c == '(' || c == ')' || c == '\\'){
   Value[q++] = '\\';
   Value[q++] = c;

  }else if(c  < ' ' || c  > '~'){
   Value[q++] = '\\';
   c = c;
   Value[q+2] = (c % 8) + '0';
   c /= 8;
   Value[q+1] = (c % 8) + '0';
   c /= 8;
   Value[q  ] = (c    ) + '0';
   q += 3;

  }else{
   Value[q++] = c;
  }
  j++;
 }

 Value[q] = 0;
}
//------------------------------------------------------------------------------

void pdfString::Set(
 int Year,
 int Month,
 int Day,
 int Hour,
 int Minute,
 int Second
){
 if(Value) delete[] Value;
 Value = new char[17];

 Value[ 0] = 'D';
 Value[ 1] = ':';

 Value[ 5] = Year % 10; Year /= 10;
 Value[ 4] = Year % 10; Year /= 10;
 Value[ 3] = Year % 10; Year /= 10;
 Value[ 2] = Year % 10;

 Value[ 7] = Month % 10; Month /= 10;
 Value[ 6] = Month % 10;

 Value[ 9] = Day % 10; Day /= 10;
 Value[ 8] = Day % 10;

 Value[11] = Hour % 10; Hour /= 10;
 Value[10] = Hour % 10;

 Value[13] = Minute % 10; Minute /= 10;
 Value[12] = Minute % 10;

 Value[15] = Second % 10; Second /= 10;
 Value[14] = Second % 10;

 Value[16] = 0;
}
//------------------------------------------------------------------------------

bool pdfString::Empty(){
 if(!Value   ) return true;
 if( Value[0]) return false;
 return true;
}
//------------------------------------------------------------------------------


int pdfString::GetLength(){
 if(!Value) return 0;

 int r = 0;
 while(Value[r]) r++;
 return r+2;
}
//------------------------------------------------------------------------------

int pdfString::GetOutput(char* Buffer){
 if(!Value) return 0;

 int i = 0;

 Buffer[0] = '(';
 while(Value[i]){
  Buffer[i+1] = Value[i];
  i++;
 }
 i++;
 Buffer[i++] = ')';

 return i;
}
//------------------------------------------------------------------------------

