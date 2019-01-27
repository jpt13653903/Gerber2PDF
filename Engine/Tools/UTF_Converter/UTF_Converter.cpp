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

// This is done manually because std:wstring_convert throws funny errors
//------------------------------------------------------------------------------

#include "UTF_Converter.h"
//------------------------------------------------------------------------------

UTF_CONVERTER UTF_Converter;
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

UTF_CONVERTER::UTF_CONVERTER(){
}
//------------------------------------------------------------------------------

UTF_CONVERTER::~UTF_CONVERTER(){
}
//------------------------------------------------------------------------------

char32_t UTF_CONVERTER::GetUTF_32(const char* UTF8, int* CodeLength){
  uint32_t n;
  uint32_t Bits;   // Valid bits
  uint32_t UTF32; // The result
  uint8_t  Lead;   // The leading byte
  const uint8_t* s = (const uint8_t*)UTF8;

  if((s[0] & 0xC0) == 0xC0){
    n     = 1;
    Lead  = s[0] << 1;
    Bits  = 0x3F; // 6 bits
    UTF32 = s[0];

    while(Lead & 0x80){
      if((s[n] & 0xC0) != 0x80){ // Invalid code-word
        if(CodeLength) *CodeLength = 1;
        return s[0];
      }
      Bits  = (Bits   << 5) |         0x1F ;
      UTF32 = (UTF32 << 6) | (s[n] & 0x3F);
      Lead <<= 1;
      n++;
    }
    if(CodeLength) *CodeLength = n;
    return UTF32 & Bits;

  }else{
    if(CodeLength) *CodeLength = 1;
    return s[0];
  }
}
//------------------------------------------------------------------------------

u32string& UTF_CONVERTER::UTF32(const char* UTF8){
  static u32string UTF32;
  UTF32.clear();

  int l;
  int n = 0;
  while(UTF8[n]){
    UTF32.append(1, GetUTF_32(UTF8+n, &l));
    n += l;
  }

  return UTF32;
}
//------------------------------------------------------------------------------

u32string& UTF_CONVERTER::UTF32(const char16_t* UTF16){
  static u32string UTF32;
  UTF32.clear();

  const uint16_t* u = (const uint16_t*)UTF16;

  char32_t c;

  for(int n = 0; u[n]; n++){
    if(
      ((u[n  ] & 0xFC00) == 0xD800) &&
      ((u[n+1] & 0xFC00) == 0xDC00)
    ){
      c =              u[n++] & 0x3FF ;
      c = (c << 10) | (u[n  ] & 0x3FF);
    }else{
      c = u[n];
    }
    UTF32.append(1, c);
  }
  return UTF32;
}
//------------------------------------------------------------------------------

u32string& UTF_CONVERTER::UTF32(const string& UTF8){
  return UTF32(UTF8.c_str());
}
//------------------------------------------------------------------------------

u32string& UTF_CONVERTER::UTF32(const u16string& UTF16){
  return UTF32(UTF16.c_str());
}
//------------------------------------------------------------------------------

u16string& UTF_CONVERTER::UTF16(const char* UTF8){
  return UTF16(UTF32(UTF8));
}
//------------------------------------------------------------------------------

u16string& UTF_CONVERTER::UTF16(const char32_t* UTF32){
  static u16string UTF16;
  UTF16.clear();

  for(int n = 0; UTF32[n]; n++){
    if(UTF32[n] > 0xFFFF){
      UTF16.append(1, (char16_t)(0xD800 | (UTF32[n] >>    10)));
      UTF16.append(1, (char16_t)(0xDC00 | (UTF32[n] &  0x3FF)));
    }else{
      UTF16.append(1, (char16_t)(UTF32[n]));
    }
  }

  return UTF16;
}
//------------------------------------------------------------------------------

u16string& UTF_CONVERTER::UTF16(const string& UTF8){
  return UTF16(UTF8.c_str());
}
//------------------------------------------------------------------------------

u16string& UTF_CONVERTER::UTF16(const u32string& UTF32){
  return UTF16(UTF32.c_str());
}
//------------------------------------------------------------------------------

string& UTF_CONVERTER::UTF8(const char16_t* UTF16){
  return UTF8(UTF32(UTF16));
}
//------------------------------------------------------------------------------

string& UTF_CONVERTER::UTF8(const char32_t* UTF32){
  static string UTF8;
  UTF8.clear();

  int      n;
  uint32_t c;
  uint8_t  Head;
  uint8_t  Lead;
  uint8_t  Cont[6];

  for(int j = 0; UTF32[j]; j++){
    c    = UTF32[j];
    n    = 0;
    Head = 0x3F; // Active bits in the leading byte
    Lead = 0x80; // Leading byte

    if(c < 0x80){
      UTF8.append(1, (char)c);

    }else{
      while(c > Head){ // Doesn't fit in the leading byte
        Cont[n] = 0x80 | (c & 0x3F);
        Lead    = 0x80 | (Lead >> 1);
        Head  >>= 1;
        c     >>= 6;
        n++;
      }
      UTF8.append(1, (char)(Lead | c));
      for(n--; n >= 0; n--) UTF8.append(1, (char)(Cont[n]));
    }
  }

  return UTF8;
}
//------------------------------------------------------------------------------

string& UTF_CONVERTER::UTF8(const u16string& UTF16){
  return UTF8(UTF16.c_str());
}
//------------------------------------------------------------------------------

string& UTF_CONVERTER::UTF8(const u32string& UTF32){
  return UTF8(UTF32.c_str());
}
//------------------------------------------------------------------------------

