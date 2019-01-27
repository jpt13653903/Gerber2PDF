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

#ifndef JFile_H
#define JFile_H
//---------------------------------------------------------------------------

#if (defined __WIN32__ ) || \
    (defined __WIN64__ ) || \
    (defined __WIN32   ) || \
    (defined __WIN64   ) || \
    (defined _WIN32    ) || \
    (defined _WIN64    ) || \
    (defined _MSC_VER  )

  #ifndef WINVER
    #define  WINVER        0x0601 // Windows 7
  #endif
  #ifndef NTDDI_VERSION
    #define  NTDDI_VERSION 0x06010000
  #endif
  #ifndef _WIN32_IE
    #define  _WIN32_IE     WINVER
  #endif
  #ifndef _WIN32_WINNT
    #define  _WIN32_WINNT  WINVER
  #endif
  #ifndef UNICODE
    #define UNICODE
  #endif

  #include <windows.h>

#elif defined(__linux__)
  #include <stdio.h>
  #include <stdint.h>
  #include <string.h>
  #include <errno.h>

#else
  #error JFile.cpp/h is platform dependant, \
         please create an implimentation for your platform
#endif
//---------------------------------------------------------------------------

#include <string>
#include "UTF_Converter.h"
//---------------------------------------------------------------------------

class JFile{
  public:
    enum ACCESS{
      Read        = 0,
      Write       = 1,
      Create      = 2,
      WriteDevice = 3
    };
//---------------------------------------------------------------------------

  private:
    #if defined(WINVER)
      HANDLE Handle;
    #elif defined(__linux__)
      FILE*  Handle;
    #endif
    ACCESS      CurrentAccess;
    std::string Filename;
//---------------------------------------------------------------------------

    bool LineInput(std::string* s);
    bool LinePrint(const char*  s);
//---------------------------------------------------------------------------

  public:
    JFile();
   ~JFile();

    int  FormatLastError(std::string* Error);
    void ShowLastError  ();
    bool Open           (ACCESS       Access);
    bool ReadLine       (std::string* Line);
    bool WriteLine      (const char*  Value);

    unsigned long ReadBuffer(
      char*         Buffer,
      unsigned long MustRead,
      bool*         Value
    );
    unsigned long WriteBuffer(
      const char*   Buffer,
      unsigned long MustWrite,
      bool*         Value
    );

    void Close();
//---------------------------------------------------------------------------

    const char* GetFilename  ();
    void        SetFilename  (const char* Value);
    long double GetSize      ();
};
//---------------------------------------------------------------------------
#endif


