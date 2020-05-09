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
//------------------------------------------------------------------------------

bool TestDebugMessages(){
  Start("Debug Messages");

  debug  ("Testing debug %d"  , 123);
  info   ("Testing info %d"   , 123);
  warning("Testing warning %d", 123);
  error  ("Testing error %d"  , 123);
  assert (true );
  assert (true , printf("This should never print\n"));
  assert (false);
  assert (false, info("Do something about the failed assertion"));
  assert (!strcmp("foo", "bar"));

  error("Testing GetErrorString(): \"%s\"", GetErrorString(123));

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  SetupTerminal();

  printf("\n\n");
  if(!TestDebugMessages()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

