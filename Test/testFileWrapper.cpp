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

#include "test.h"
#include "FileWrapper.h"
//------------------------------------------------------------------------------

bool TestFileWrapper(){
  Start("FILE_WRAPPER Class");

  #ifdef WINVER
    const  char  * FileName_UTF8  =  "Resources/Lorem Ipsum.txt";
    const wchar_t* FileName_UTF16 = L"Resources/Λορεμ Ιπσθμ.txt";
  #else
    const char* FileName_UTF8  = "Resources/Lorem Ipsum.txt";
    const char* FileName_Greek = "Resources/Λορεμ Ιπσθμ.txt";
  #endif

  info("UTF-8 encoded file name...");
  FILE_WRAPPER File;
  if(!File.Open(FileName_UTF8, FILE_WRAPPER::faRead)){
    error("Cannot open file \"%s\"", FileName_UTF8);
    return false;
  }
  uint64_t Size = File.GetSize();
  info("File size = %u", (unsigned)Size);
  char* Buffer = new char[Size+1];

  File.Read(Buffer, Size); Buffer[Size] = 0;
  info("File contents:\n\"\"\"\n%s\n\"\"\"", Buffer);

  delete[] Buffer;
  File.Close();
  Done();

  #ifdef WINVER
    info("UTF-16 encoded file name...");
    if(!File.Open(FileName_UTF16, FILE_WRAPPER::faRead)){
      error("Cannot open file \"%s\"", "Resources/Λορεμ Ιπσθμ.txt");
      return false;
    }
  #else
    info("Greek file name...");
    if(!File.Open(FileName_Greek, FILE_WRAPPER::faRead)){
      error("Cannot open file \"%s\"", "Resources/Λορεμ Ιπσθμ.txt");
      return false;
    }
  #endif
  Size = File.GetSize();
  info("File size = %u", (unsigned)Size);
  Buffer = new char[Size+1];

  File.Read(Buffer, Size); Buffer[Size] = 0;
  info("File contents:\n\"\"\"\n%s\n\"\"\"", Buffer);

  delete[] Buffer;
  File.Close();

  Done(); return true;
}
//------------------------------------------------------------------------------

bool TestPathCreation(){
  Start("Path Creation");

  FILE_WRAPPER File;

  assert(File.WriteAll("testOutput/A/B/C/D/E/File.txt", (const byte*)"Hello World!\n"), return false);

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  SetupTerminal();

  printf("\n\n");
  if(!TestFileWrapper ()) goto main_Error;
  if(!TestPathCreation()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

