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

#ifndef FileWrapper_h
#define FileWrapper_h
//------------------------------------------------------------------------------

#include "General.h"
#include "UnicodeString.h"
//------------------------------------------------------------------------------

#ifndef WINVER
  #error FileWrapper is Windows-only at the moment...
#endif
//------------------------------------------------------------------------------

class FileWrapper{
  public:
    enum ACCESS{
      // Normal buffered
      faRead,
      faWrite,
      faCreate
    };
  private:
    HANDLE Handle;

  public:
    FileWrapper();
   ~FileWrapper();

    bool Open (const  char  * Filename, ACCESS Access); // UTF-8
    bool Open (const wchar_t* Filename, ACCESS Access);
    void Close();

    uint64_t GetSize();

    unsigned Read (      char* Buffer, unsigned MustRead);
    unsigned Write(const char* Buffer); // Null-terminated
    unsigned Write(const char* Buffer, unsigned MustWrite);
    bool     Flush();

    void GetTime(FILETIME* Creation, FILETIME* Access, FILETIME* Modified);
    void SetTime(
      const FILETIME* Creation,
      const FILETIME* Access,
      const FILETIME* Modified
    );
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
