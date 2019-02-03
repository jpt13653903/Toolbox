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

#ifndef WINVER
  #define INVALID_HANDLE_VALUE 0
  #define FILETIME time_t
#endif
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

  #ifdef WINVER
    return Open((const wchar_t*)UTF_Converter.UTF16(Filename).c_str(), Access);

  #else
    if(Handle) Close();
    this->Filename = Filename;

    switch(Access){
      case faRead:
        Handle = fopen(Filename, "rb");
        break;

      case faWrite:
        Handle = fopen(Filename, "wb");
        if(Handle) fseek(Handle, 0, SEEK_END);
        break;

      case faCreate:
        Handle = fopen(Filename, "wb");
        break;

      default:
        break;
    }
    return Handle;
  #endif
}
//------------------------------------------------------------------------------

bool FILE_WRAPPER::Open(const wchar_t* Filename, ACCESS Access){
  #ifdef WINVER
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

  #else // Not Windows
    return false;
  #endif
}
//------------------------------------------------------------------------------

void FILE_WRAPPER::Close(){
  #ifdef WINVER
    if(Handle == INVALID_HANDLE_VALUE) return;
    CloseHandle(Handle);
    Handle = INVALID_HANDLE_VALUE;
  #else
    if(Handle) fclose(Handle);
    Handle = 0;
  #endif
}
//------------------------------------------------------------------------------

uint64_t FILE_WRAPPER::GetSize(){
  if(Handle == INVALID_HANDLE_VALUE) return 0;

  #if defined(WINVER)
    uint64_t result;
    DWORD    high = 0;

    result  = GetFileSize(Handle, &high);
    result += ((uint64_t)high) << 32;
    return result;

  #elif defined(__linux__)
    long int position = ftell(Handle);
    fseek(Handle, 0, SEEK_END);
    long int size = ftell(Handle);
    fseek(Handle, position, SEEK_SET);
    return size;
  #endif
}
//------------------------------------------------------------------------------

unsigned FILE_WRAPPER::Read(char* Buffer, unsigned MustRead){
  if(Handle == INVALID_HANDLE_VALUE) return 0;

  #ifdef WINVER
    DWORD Length;
    if(!ReadFile(Handle, Buffer, MustRead, &Length, 0)) return 0;
    return Length;
  #else
    return fread(Buffer, 1, MustRead, Handle);
  #endif
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

  #ifdef WINVER
    DWORD Length;
    if(!WriteFile(Handle, Buffer, MustWrite, &Length, 0)) return 0;
    return Length;
  #else
    return fwrite(Buffer, 1, MustWrite, Handle);
  #endif
}
//------------------------------------------------------------------------------

bool FILE_WRAPPER::Flush(){
  if(Handle == INVALID_HANDLE_VALUE) return false;

  #ifdef WINVER
    return FlushFileBuffers(Handle);
  #else
    return !fflush(Handle);
  #endif
}
//------------------------------------------------------------------------------

void FILE_WRAPPER::GetTime(
  FILETIME* Creation,
  FILETIME* Access,
  FILETIME* Modified
){
  if(Handle == INVALID_HANDLE_VALUE) return;

  #ifdef WINVER
    GetFileTime(Handle, Creation, Access, Modified);
  #else
    struct stat Stat;
    if(!stat(Filename.c_str(), &Stat)){
      *Creation = Stat.st_ctime;
      *Access   = Stat.st_atime;
      *Modified = Stat.st_mtime;
    }
  #endif
}
//------------------------------------------------------------------------------

void FILE_WRAPPER::SetTime(
  const FILETIME* Creation,
  const FILETIME* Access,
  const FILETIME* Modified
){
  if(Handle == INVALID_HANDLE_VALUE) return;

  #ifdef WINVER
    SetFileTime(Handle, Creation, Access, Modified);
  #else
    struct utimbuf Time;
    Time.actime  = *Access;
    Time.modtime = *Modified;
    utime(Filename.c_str(), &Time);
  #endif
}
//------------------------------------------------------------------------------
