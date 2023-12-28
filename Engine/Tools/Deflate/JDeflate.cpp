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

#include <stdio.h>
#include <vector>
//------------------------------------------------------------------------------

#include "zlib.h"
#include "General.h"
#include "JDeflate.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

JDeflate::JDeflate(){
}
//------------------------------------------------------------------------------

unsigned char* JDeflate::Deflate(unsigned char* Buffer, unsigned* Length)
{
  const int CHUNK = 64<<10;

  int ret;
  unsigned char out[CHUNK];

  z_stream Stream;
  Stream.zalloc = Z_NULL;
  Stream.zfree  = Z_NULL;
  Stream.opaque = Z_NULL;
  ret = deflateInit(&Stream, Z_BEST_COMPRESSION);
  if(ret != Z_OK) return 0;

  Stream.next_in = Buffer;
  Stream.avail_in = *Length;

  vector<unsigned char> Result;

  do{
    Stream.avail_out = CHUNK;
    Stream.next_out = out;
    ret = deflate(&Stream, Z_FINISH);
    assert(ret != Z_STREAM_ERROR);
    unsigned have = CHUNK - Stream.avail_out;

    for(unsigned n = 0; n < have; n++) Result.push_back(out[n]);
  } while (Stream.avail_out == 0);

  assert(Stream.avail_in == 0);
  assert(ret == Z_STREAM_END);

  deflateEnd(&Stream);

  int n  = 0;
  Buffer = new unsigned char[Result.size()];
  for(auto Element: Result) Buffer[n++] = Element;
  *Length = n;

  return Buffer;
}
//------------------------------------------------------------------------------

unsigned char* JDeflate::Inflate(unsigned char* Buffer, unsigned* Length)
{
  const int CHUNK = 64<<10;

  int ret;
  unsigned char out[CHUNK];

  z_stream Stream;
  Stream.zalloc   = Z_NULL;
  Stream.zfree    = Z_NULL;
  Stream.opaque   = Z_NULL;
  Stream.avail_in = 0;
  Stream.next_in  = Z_NULL;
  ret = inflateInit(&Stream);
  if(ret != Z_OK) return 0;

  Stream.next_in  = Buffer;
  Stream.avail_in = *Length;

  vector<unsigned char> Result;

  do{
    Stream.avail_out = CHUNK;
    Stream.next_out = out;
    ret = inflate(&Stream, Z_FINISH);
    assert(ret != Z_STREAM_ERROR);
    unsigned have = CHUNK - Stream.avail_out;

    for(unsigned n = 0; n < have; n++) Result.push_back(out[n]);
  } while (Stream.avail_out == 0);

  assert(Stream.avail_in == 0);
  assert(ret == Z_STREAM_END);

  inflateEnd(&Stream);

  int n  = 0;
  Buffer = new unsigned char[Result.size()];
  for(auto Element: Result) Buffer[n++] = Element;
  *Length = n;

  return Buffer;
}
//------------------------------------------------------------------------------
