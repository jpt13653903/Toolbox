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
#include "UTF_Converter.h"
//------------------------------------------------------------------------------

#if   defined(WINVER)
#elif defined(__linux__)
#else
  #error FileWrapper is not implemented for your platform
#endif
//------------------------------------------------------------------------------

class FILE_WRAPPER{
  public:
    enum ACCESS{
      faRead,
      faWrite,
      faCreate
    };
  private:
    #if defined(WINVER)
      HANDLE Handle;
    #elif defined(__linux__)
      std::string Filename;
      FILE*       Handle;
    #endif

  public:
    FILE_WRAPPER();
   ~FILE_WRAPPER();

    bool Open (const  char  * Filename, ACCESS Access); // UTF-8
    bool Open (const wchar_t* Filename, ACCESS Access); // Windows only: UTF-16
    void Close();

    uint64_t GetSize();

    unsigned Read (      char* Buffer, unsigned MustRead);
    unsigned Write(const char* Buffer); // Null-terminated
    unsigned Write(const char* Buffer, unsigned MustWrite);
    bool     Flush();

    #ifdef WINVER
      void GetTime(FILETIME* Creation, FILETIME* Access, FILETIME* Modified);
      void SetTime(
        const FILETIME* Creation,
        const FILETIME* Access,
        const FILETIME* Modified
      );
    #else
      void GetTime(time_t* StatusChange, time_t* Access, time_t* Modified);
      void SetTime(
        const time_t* StatusChange,
        const time_t* Access,
        const time_t* Modified
      );
    #endif
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
