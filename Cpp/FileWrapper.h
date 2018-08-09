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
