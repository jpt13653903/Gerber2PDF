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

#ifndef UTF_Converter_H
#define UTF_Converter_H
//------------------------------------------------------------------------------

#include <stdint.h>
#include <string>
//------------------------------------------------------------------------------

class UTF_CONVERTER{
  private:
    std::u32string UTF_32;
    std::u16string UTF_16;
    std::string    UTF_8;

    char32_t GetUTF_32(const char* UTF_8, int* CodeLength);

  public:
    UTF_CONVERTER();
   ~UTF_CONVERTER();

    std::u32string& UTF32(const char*           UTF_8 );
    std::u32string& UTF32(const char16_t*       UTF_16);
    std::u32string& UTF32(const std::string&    UTF_8 );
    std::u32string& UTF32(const std::u16string& UTF_16);

    std::u16string& UTF16(const char*           UTF_8 );
    std::u16string& UTF16(const char32_t*       UTF_32);
    std::u16string& UTF16(const std::string&    UTF_8 );
    std::u16string& UTF16(const std::u32string& UTF_32);

    std::string&    UTF8 (const char16_t*       UTF_16);
    std::string&    UTF8 (const char32_t*       UTF_32);
    std::string&    UTF8 (const std::u16string& UTF_16);
    std::string&    UTF8 (const std::u32string& UTF_32);
};
//------------------------------------------------------------------------------

extern UTF_CONVERTER UTF_Converter;
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

