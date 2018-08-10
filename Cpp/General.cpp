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

