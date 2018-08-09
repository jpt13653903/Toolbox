#define DEBUG
//------------------------------------------------------------------------------

#include "General.h"
#include "FileWrapper.h"
//------------------------------------------------------------------------------

void Start(const char* Message){
  info("%s", Message); printf("\n");
}
//------------------------------------------------------------------------------

void Done(){
  printf(ANSI_FG_BRIGHT_BLACK
         "----------------------------------------"
         "----------------------------------------\n\n");
}
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

bool TestFileWrapper(){
  Start("FileWrapper Class");

  const  char  * FileName_UTF8  =  "Resources/Lorem Ipsum.txt";
  const wchar_t* FileName_UTF16 = L"Resources/Λορεμ Ιπσθμ.txt";

  info("UTF-8 encoded file name...");
  FileWrapper File;
  if(!File.Open(FileName_UTF8, FileWrapper::faRead)){
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

  info("UTF-16 encoded file name...");
  if(!File.Open(FileName_UTF16, FileWrapper::faRead)){
    error("Cannot open file \"%s\"", FileName_UTF8);
    return false;
  }
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

int main(){
  printf("\n\n");
  if(!TestDebugMessages()) return -1;
  if(!TestFileWrapper  ()) return -1;

  info(ANSI_FG_GREEN "All OK");
  return 0;
}
//------------------------------------------------------------------------------

