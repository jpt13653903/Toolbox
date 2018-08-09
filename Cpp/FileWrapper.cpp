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

#include "FileWrapper.h"
//------------------------------------------------------------------------------

FileWrapper::FileWrapper(){
  Handle = INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

FileWrapper::~FileWrapper(){
  Close();
}
//------------------------------------------------------------------------------

bool FileWrapper::Open(const char* Filename, ACCESS Access){
  if(!Filename || !Filename[0]) return false;

  UnicodeString Codec;
  Codec = Filename;

  Open((const wchar_t*)Codec.UTF16(), Access);

  return Handle != INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

bool FileWrapper::Open(const wchar_t* Filename, ACCESS Access){
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

void FileWrapper::Close(){
  if(Handle == INVALID_HANDLE_VALUE) return;
  CloseHandle(Handle);
  Handle = INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

uint64_t FileWrapper::GetSize(){
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

unsigned FileWrapper::Read(char* Buffer, unsigned MustRead){
  if(Handle == INVALID_HANDLE_VALUE) return 0;

  DWORD Length;
  if(!ReadFile(Handle, Buffer, MustRead, &Length, 0)) return 0;
  return Length;
}
//------------------------------------------------------------------------------

unsigned FileWrapper::Write(const char* Buffer){
  int j;
  for(j = 0; Buffer[j]; j++);
  return Write(Buffer, j);
}
//------------------------------------------------------------------------------

unsigned FileWrapper::Write(const char* Buffer, unsigned MustWrite){
  if(Handle == INVALID_HANDLE_VALUE) return 0;

  DWORD Length;
  if(!WriteFile(Handle, Buffer, MustWrite, &Length, 0)) return 0;
  return Length;
}
//------------------------------------------------------------------------------

bool FileWrapper::Flush(){
  if(Handle == INVALID_HANDLE_VALUE) return false;

  return FlushFileBuffers(Handle);
}
//------------------------------------------------------------------------------

void FileWrapper::GetTime(
  FILETIME* Creation,
  FILETIME* Access,
  FILETIME* Modified
){
  if(Handle == INVALID_HANDLE_VALUE) return;

  GetFileTime(Handle, Creation, Access, Modified);
}
//------------------------------------------------------------------------------

void FileWrapper::SetTime(
  const FILETIME* Creation,
  const FILETIME* Access,
  const FILETIME* Modified
){
  if(Handle == INVALID_HANDLE_VALUE) return;

  SetFileTime(Handle, Creation, Access, Modified);
}
//------------------------------------------------------------------------------
