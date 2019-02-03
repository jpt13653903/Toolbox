//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#include "FileWrapper.h"
//------------------------------------------------------------------------------

FILE_WRAPPER::FILE_WRAPPER(){
  Handle = INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

FILE_WRAPPER::~FILE_WRAPPER(){
  Close();
}
//------------------------------------------------------------------------------

bool FILE_WRAPPER::Open(const char* Filename, ACCESS Access){
  if(!Filename || !Filename[0]) return false;

  Open((const wchar_t*)UTF_Converter.UTF16(Filename).c_str(), Access);

  return Handle != INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

bool FILE_WRAPPER::Open(const wchar_t* Filename, ACCESS Access){
  if(!Filename || !Filename[0]) return false;

  if(Handle != INVALID_HANDLE_VALUE) Close();

  DWORD DesiredAccess = 0;
  DWORD ShareMode     = 0;
  DWORD Creation      = 0;

  switch(Access){
    case faRead:
      DesiredAccess = GENERIC_READ;
      ShareMode     = FILE_SHARE_READ;
      Creation      = OPEN_EXISTING;
      break;

    case faWrite:
      DesiredAccess = GENERIC_WRITE;
      Creation      = OPEN_EXISTING;
      break;

    case faCreate:
      DesiredAccess = GENERIC_WRITE;
      Creation      = CREATE_ALWAYS;
      break;

    default:
      break;
  }

  int j, q;
  for(j = 0; Filename[j]; j++);

  wchar_t* LongFile = new wchar_t[j+5];

  q = 0;

  if(Filename[1] == ':'){
    LongFile[q++] = '\\';
    LongFile[q++] = '\\';
    LongFile[q++] = '?';
    LongFile[q++] = '\\';
  }
  for(j = 0; Filename[j]; j++) LongFile[q++] = Filename[j];
  LongFile[q] = 0;

  Handle = CreateFileW(
    LongFile,
    DesiredAccess,
    ShareMode,
    0,
    Creation,
    FILE_ATTRIBUTE_NORMAL,
    0
  );
  delete[] LongFile;

  if(Handle == INVALID_HANDLE_VALUE) return false;

  if(Access == faWrite) SetFilePointer(Handle, 0, 0, FILE_END);

  return true;
}
//------------------------------------------------------------------------------

void FILE_WRAPPER::Close(){
  if(Handle == INVALID_HANDLE_VALUE) return;
  CloseHandle(Handle);
  Handle = INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

uint64_t FILE_WRAPPER::GetSize(){
  uint64_t result;
  DWORD    high = 0;

  if(Handle){
    result  = GetFileSize(Handle, &high);
    result += ((uint64_t)high) << 32;
    return result;
  }
  return 0;
}
//------------------------------------------------------------------------------

unsigned FILE_WRAPPER::Read(char* Buffer, unsigned MustRead){
  if(Handle == INVALID_HANDLE_VALUE) return 0;

  DWORD Length;
  if(!ReadFile(Handle, Buffer, MustRead, &Length, 0)) return 0;
  return Length;
}
//------------------------------------------------------------------------------

unsigned FILE_WRAPPER::Write(const char* Buffer){
  int j;
  for(j = 0; Buffer[j]; j++);
  return Write(Buffer, j);
}
//------------------------------------------------------------------------------

unsigned FILE_WRAPPER::Write(const char* Buffer, unsigned MustWrite){
  if(Handle == INVALID_HANDLE_VALUE) return 0;

  DWORD Length;
  if(!WriteFile(Handle, Buffer, MustWrite, &Length, 0)) return 0;
  return Length;
}
//------------------------------------------------------------------------------

bool FILE_WRAPPER::Flush(){
  if(Handle == INVALID_HANDLE_VALUE) return false;

  return FlushFileBuffers(Handle);
}
//------------------------------------------------------------------------------

void FILE_WRAPPER::GetTime(
  FILETIME* Creation,
  FILETIME* Access,
  FILETIME* Modified
){
  if(Handle == INVALID_HANDLE_VALUE) return;

  GetFileTime(Handle, Creation, Access, Modified);
}
//------------------------------------------------------------------------------

void FILE_WRAPPER::SetTime(
  const FILETIME* Creation,
  const FILETIME* Access,
  const FILETIME* Modified
){
  if(Handle == INVALID_HANDLE_VALUE) return;

  SetFileTime(Handle, Creation, Access, Modified);
}
//------------------------------------------------------------------------------
