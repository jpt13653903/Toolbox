#include "test.h"
//------------------------------------------------------------------------------

bool TestDebugMessages(){
  Start("Debug Messages");

  debug  ("Testing debug %d"  , 123);
  info   ("Testing info %d"   , 123);
  warning("Testing warning %d", 123);
  error  ("Testing error %d"  , 123);

  error("Testing GetErrorString(): \"%s\"", GetErrorString(123));

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  printf("\n\n");
  if(!TestDebugMessages()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

