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

#include "General.h"
//------------------------------------------------------------------------------

#ifdef WINVER
  static char ErrorString[0x1000];
  
  const char* GetErrorString(int Code){
    ErrorString[0] = 0;

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, Code, 0, ErrorString, 0x1000, 0);

    int n = strlen(ErrorString);
    for(n = strlen(ErrorString)-1; n >= 0; n--){
      switch(ErrorString[n]){
        case ' ' :
        case '\t':
        case '\r':
        case '\n': ErrorString[n] = 0; break;
        default  : return ErrorString;
      }
    }
    return ErrorString;
  }
#else
  const char* GetErrorString(int Code){ return strerror(Code); }
#endif
//------------------------------------------------------------------------------

#ifndef WINVER
  uint32_t GetTickCount(){
    struct timespec now;
    if(clock_gettime(CLOCK_MONOTONIC, &now)) return 0;
    return now.tv_sec * 1000LL + now.tv_nsec / 1000000LL;
  }
  uint64_t GetTickCount64(){
    struct timespec now;
    if(clock_gettime(CLOCK_MONOTONIC, &now)) return 0;
    return now.tv_sec * 1000LL + now.tv_nsec / 1000000LL;
  }
#endif
//------------------------------------------------------------------------------       

