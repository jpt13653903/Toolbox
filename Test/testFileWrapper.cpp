#include "test.h"
#include "FileWrapper.h"
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
    error("Cannot open file \"%s\"", "Resources/Λορεμ Ιπσθμ.txt");
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
  if(!TestFileWrapper()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------
