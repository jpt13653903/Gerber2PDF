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

#include "JFile.h"
//---------------------------------------------------------------------------

JFile::JFile(){
  Filename.Set("");
  Handle        = 0;
  CurrentAccess = Read;
}
//---------------------------------------------------------------------------

#ifdef __linux__
  bool ReadFile(
    FILE*          File,
    void*          Buffer,
    unsigned long  NumberOfBytesToRead,
    unsigned long* NumberOfBytesRead,
    void*          Overlapped
  ){
    *NumberOfBytesRead = fread(Buffer, 1, NumberOfBytesToRead, File);
    return *NumberOfBytesRead;
  }
#endif
//---------------------------------------------------------------------------

#ifdef __linux__
  bool WriteFile(
    FILE*          File,
    const void*    Buffer,
    unsigned long  NumberOfBytesToWrite,
    unsigned long* NumberOfBytesWritten,
    void*          Overlapped
  ){
    *NumberOfBytesWritten = fwrite(Buffer, 1, NumberOfBytesToWrite, File);
    return *NumberOfBytesWritten;
  }
#endif
//---------------------------------------------------------------------------

#ifdef __linux__
  void CloseHandle(FILE*  Handle){
    fclose(Handle);
  }
#endif
//---------------------------------------------------------------------------

bool JFile::LineInput(JString* s){
  unsigned long X;
  char          x[2];
  s->Set("");
  if (!ReadFile(Handle, x, 1, &X, 0)) return false;
  if (X <= 0) return false;
  x[1] = x[0];
  while(true){
    if (!ReadFile(Handle, x, 1, &X, 0)) return true;
    if (X <= 0){
      s->Append(x[1]);
      return true;
    }
    if (((int)x[1] == 13 && (int)x[0] == 10) ||  // return + line feed
         (int)x[1] == 10 ||                      // line feed
         (int)x[1] == 26) return true;           // end of file
    s->Append(x[1]);
    x[1] = x[0];
  }
}
//---------------------------------------------------------------------------

bool JFile::LinePrint(const char* s){
  unsigned long X;
  JString       x;
  x.Set(s);
  x.Append("\r\n");
  return WriteFile(Handle, x.String, x.GetLength(), &X, 0);
}
//---------------------------------------------------------------------------

JFile::~JFile(void){
  Close();
}
//---------------------------------------------------------------------------

void JFile::Close(void){
  if (Handle){
    CloseHandle(Handle);
    Handle = 0;
  }
}
//---------------------------------------------------------------------------

bool JFile::Open(ACCESS Access){
  Close();

  #if defined(WINVER)
    switch(Access){
      case Read:
        Handle = CreateFile(
          Filename.String,
          GENERIC_READ,
          FILE_SHARE_READ,
          0,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,
          0
        );
        if(Handle == INVALID_HANDLE_VALUE){
          Handle = 0;
          return false;
        }
        CurrentAccess = Read;
        break;
      case Write:
        Handle = CreateFile(
          Filename.String,
          GENERIC_WRITE,
          FILE_SHARE_READ,
          0,
          OPEN_ALWAYS,
          FILE_ATTRIBUTE_NORMAL,
          0
        );
        if(Handle == INVALID_HANDLE_VALUE){
          Handle = 0;
          return false;
        }
        CurrentAccess = Write;
        break;
      case Create:
        Handle = CreateFile(
          Filename.String,
          GENERIC_WRITE,
          FILE_SHARE_READ,
          0,
          CREATE_ALWAYS,
          FILE_ATTRIBUTE_NORMAL,
          0
        );
        if(Handle == INVALID_HANDLE_VALUE){
          Handle = 0;
          return false;
        }
        CurrentAccess = Create;
        break;
      case WriteDevice:
        Handle = CreateFile(
          Filename.String,
          GENERIC_WRITE,
          FILE_SHARE_READ | FILE_SHARE_WRITE,
          0,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,
          0
        );
        if(Handle == INVALID_HANDLE_VALUE){
          Handle = 0;
          return false;
        }
        CurrentAccess = Write;
        break;
      default:
        return false;
    }
  #elif defined(__linux__)
    switch(Access){
      case Read:
        Handle = fopen(Filename.String, "rb");
        if(!Handle) return false;
        CurrentAccess = Read;
        break;

      case Write:
        Handle = fopen(Filename.String, "wb");
        if(!Handle) return false;
        CurrentAccess = Write;
        break;

      case Create:
        Handle = fopen(Filename.String, "wb");
        if(!Handle) return false;
        CurrentAccess = Create;
        break;

      case WriteDevice:
        printf("JFile.cpp Error: Write Device not implemented for Linux.");
        return false;

      default:
        return false;
    }
  #endif

  return true;
}
//---------------------------------------------------------------------------

int JFile::FormatLastError(JString* Error){
  #if defined(WINVER)
    char Buffer[0x100];
    int Err = GetLastError();
  
    FormatMessage((unsigned long)FORMAT_MESSAGE_FROM_SYSTEM,
                  (const void*)FORMAT_MESSAGE_FROM_HMODULE,
                  (unsigned long)Err,
                  (unsigned long)0,
                  (char*)Buffer,
                  (unsigned long)0x100,
  		0);
    Error->Set(Err);
    Error->Append(": ");
    Error->Append(Buffer);
  
    return Err;
  
  #elif defined(__linux__)
    Error->Set(errno);
    Error->Append(": ");
    Error->Append(strerror(errno));

    return errno;
  #endif
}
//------------------------------------------------------------------------------

void JFile::ShowLastError(){
  int Err;
  JString s;

  Err = FormatLastError(&s);

  #if defined(WINVER)
    s.Prefix(":\r\n");
    s.Prefix(Filename.String);
  
    if(Err){
      MessageBox(
        0,
        s.String,
        "Error",
        MB_ICONERROR
      );
    }else{
      MessageBox(
        0,
        s.String,
        "Info",
        MB_ICONINFORMATION
      );
    }
  #elif defined(__linux__)
    if(Err) printf("Error:\n  %s\n  %s", s.String, Filename.String);
    else    printf("Info:\n  %s\n  %s" , s.String, Filename.String);
  #endif
}
//---------------------------------------------------------------------------

char* JFile::GetFilename(){
  return Filename.String;
}
//---------------------------------------------------------------------------

void JFile::SetFilename(const char* Value){
  Close();

  Filename.Set(Value);

  #if defined(WINVER)
    if(Filename.GetLength() < 3){
      Filename.Set("");
      return;
    }
    if((Value[2] != ':'  || Value[3] != '\\') &&
       (Value[1] != '\\' && Value[2] != '\\')){
      JString t;
      t.Set("Invalid path and filename: ");
      t.Append(Value);
      t.Append('.');
      MessageBox(0, t.String, "Error", MB_ICONERROR);
      Filename.Set("");
      return;
    }
  #endif
}
//---------------------------------------------------------------------------

bool JFile::ReadLine(JString* Line){
  if(!Handle){
    Line->Set("");
    return false;
  }
  if(!CurrentAccess){
    return LineInput(Line);
  }else{
    Line->Set("");
    return false;
  }
}
//---------------------------------------------------------------------------

bool JFile::WriteLine(const char* Value){
  if(!Handle) return false;
  if(CurrentAccess){
    return LinePrint(Value);
  }
  return false;
}
//---------------------------------------------------------------------------

unsigned long JFile::ReadBuffer(
  char*         Buffer,
  unsigned long MustRead,
  bool*         Value
){
  if(!Handle){
    *Value = false;
    return 0;
  }
  unsigned long X = 0;
  if (!CurrentAccess){
    *Value = ReadFile(Handle, Buffer, MustRead, &X, 0);
  }else{
    *Value = false;
  }
  return X;
}
//---------------------------------------------------------------------------

unsigned long JFile::WriteBuffer(
  const char*   Buffer,
  unsigned long MustWrite,
  bool*         Value
){
  if(!Handle){
    *Value = false;
    return 0;
  }
  unsigned long X = 0;
  if (CurrentAccess){
    *Value = WriteFile(Handle, Buffer, MustWrite, &X, 0);
  }else{
    *Value = false;
  }
  return X;
}
//---------------------------------------------------------------------------

long double JFile::GetSize(void){
  if(!Handle) return 0;

  #if defined(WINVER)
    long double   f;
    unsigned long high = 0;

    f  = GetFileSize(Handle, &high);
    f += high * 4.294967296e9;
    return f;

  #elif defined(__linux__)
    long int position = ftell(Handle);
    fseek(Handle, 0, SEEK_END);
    long int size = ftell(Handle);
    fseek(Handle, position, SEEK_SET);
    return size;
  #endif
}
//---------------------------------------------------------------------------

