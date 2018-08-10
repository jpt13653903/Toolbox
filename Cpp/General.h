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

#ifndef General_h
#define General_h
//------------------------------------------------------------------------------

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if (defined __WIN32 ) || \
    (defined __WIN64 ) || \
    (defined _WIN32  ) || \
    (defined _WIN64  ) || \
    (defined _MSC_VER)

  #ifndef WINVER
    #define  WINVER        0x0601 // Windows 7
    #define  NTDDI_VERSION 0x06010000
    #define  _WIN32_IE     WINVER
    #define  _WIN32_WINNT  WINVER
  #endif

  #ifndef UNICODE
    #define UNICODE
  #endif

  #include <Windows.h>

  #ifndef errno
    #define errno GetLastError()
  #endif
//------------------------------------------------------------------------------

#else // Linux
  #include <errno.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <signal.h>
  #include <pthread.h>
  #include <semaphore.h>
  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <sys/time.h>
  #include <sys/ioctl.h>
  #include <linux/string.h>

  #define  Sleep(ms) usleep(1000*(ms))
  #define  GetLastError() errno
  #define  GetErrorString(Code) strerror(Code)
  uint32_t GetTickCount();
  uint64_t GetTickCount64();
#endif
//------------------------------------------------------------------------------

#define kiB 1024
#define MiB 1048576
//------------------------------------------------------------------------------

typedef uint8_t  byte;
typedef uint16_t word;
//------------------------------------------------------------------------------

#define ANSI_RESET             "\e[0m"
#define ANSI_FG_BLACK          "\e[30m"
#define ANSI_BG_BLACK          "\e[40m"
#define ANSI_FG_RED            "\e[31m"
#define ANSI_BG_RED            "\e[41m"
#define ANSI_FG_GREEN          "\e[32m"
#define ANSI_BG_GREEN          "\e[42m"
#define ANSI_FG_YELLOW         "\e[33m"
#define ANSI_BG_YELLOW         "\e[43m"
#define ANSI_FG_BLUE           "\e[34m"
#define ANSI_BG_BLUE           "\e[44m"
#define ANSI_FG_MAGENTA        "\e[35m"
#define ANSI_BG_MAGENTA        "\e[45m"
#define ANSI_FG_CYAN           "\e[36m"
#define ANSI_BG_CYAN           "\e[46m"
#define ANSI_FG_WHITE          "\e[37m"
#define ANSI_BG_WHITE          "\e[47m"
#define ANSI_FG_BRIGHT_BLACK   "\e[90m"
#define ANSI_BG_BRIGHT_BLACK   "\e[100m"
#define ANSI_FG_BRIGHT_RED     "\e[91m"
#define ANSI_BG_BRIGHT_RED     "\e[101m"
#define ANSI_FG_BRIGHT_GREEN   "\e[92m"
#define ANSI_BG_BRIGHT_GREEN   "\e[102m"
#define ANSI_FG_BRIGHT_YELLOW  "\e[93m"
#define ANSI_BG_BRIGHT_YELLOW  "\e[103m"
#define ANSI_FG_BRIGHT_BLUE    "\e[94m"
#define ANSI_BG_BRIGHT_BLUE    "\e[104m"
#define ANSI_FG_BRIGHT_MAGENTA "\e[95m"
#define ANSI_BG_BRIGHT_MAGENTA "\e[105m"
#define ANSI_FG_BRIGHT_CYAN    "\e[96m"
#define ANSI_BG_BRIGHT_CYAN    "\e[106m"
#define ANSI_FG_BRIGHT_WHITE   "\e[97m"
#define ANSI_BG_BRIGHT_WHITE   "\e[107m"
//------------------------------------------------------------------------------

#if defined(DEBUG)
  #define debug(...) do                                            \
  {                                                                \
    printf(ANSI_FG_GREEN "Debug: " ANSI_RESET __VA_ARGS__);        \
    printf(ANSI_FG_BRIGHT_BLACK " [%s +%d \"%s(...)\"]\n"          \
           ANSI_RESET, __FILE__, __LINE__, __func__);              \
  }while(0)
#else
  #define debug(...)
#endif

#define info(...) do                                    \
{                                                       \
  printf(ANSI_FG_CYAN "Info: " ANSI_RESET __VA_ARGS__); \
  printf(ANSI_FG_BRIGHT_BLACK " [%s +%d \"%s(...)\"]\n" \
         ANSI_RESET, __FILE__, __LINE__, __func__);     \
}while(0)

#define warning(...) do                                      \
{                                                            \
  printf(ANSI_FG_YELLOW "Warning: " ANSI_RESET __VA_ARGS__); \
  printf(ANSI_FG_BRIGHT_BLACK " [%s +%d \"%s(...)\"]\n"      \
         ANSI_RESET, __FILE__, __LINE__, __func__);          \
}while(0)

#define error(...) do                                          \
{                                                              \
  printf(ANSI_FG_BRIGHT_RED "Error: " ANSI_RESET __VA_ARGS__); \
  printf(ANSI_FG_BRIGHT_BLACK " [%s +%d \"%s(...)\"]\n"        \
         ANSI_RESET, __FILE__, __LINE__, __func__);            \
}while(0)
//------------------------------------------------------------------------------

// Static allocation: do not free
const char* GetErrorString(int Code);
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
