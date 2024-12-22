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

bool testFileWrapper()
{
    start("FileWrapper Class");

    #ifdef WINVER
        const  char  * fileNameUtf8  =  "Resources/Lorem Ipsum.txt";
        const wchar_t* fileNameUtf16 = L"Resources/Λορεμ Ιπσθμ.txt";
    #else
        const char* fileNameUtf8  = "Resources/Lorem Ipsum.txt";
        const char* fileNameGreek = "Resources/Λορεμ Ιπσθμ.txt";
    #endif

    info("UTF-8 encoded file name...");
    FileWrapper file;
    if(!file.open(fileNameUtf8, FileWrapper::Access::Read)){
        error("Cannot open file \"%s\"", fileNameUtf8);
        return false;
    }
    uint64_t size = file.getSize();
    info("file size = %u", (unsigned)size);
    char* buffer = new char[size+1];

    file.read(buffer, size); buffer[size] = 0;
    info("file contents:\n\"\"\"\n%s\n\"\"\"", buffer);

    delete[] buffer;
    file.close();
    done();

    #ifdef WINVER
        info("UTF-16 encoded file name...");
        if(!file.open(fileNameUtf16, FileWrapper::Access::Read)){
            error("Cannot open file \"%s\"", "Resources/Λορεμ Ιπσθμ.txt");
            return false;
        }
    #else
        info("Greek file name...");
        if(!file.open(fileNameGreek, FileWrapper::Access::Read)){
            error("Cannot open file \"%s\"", "Resources/Λορεμ Ιπσθμ.txt");
            return false;
        }
    #endif
    size = file.getSize();
    info("file size = %u", (unsigned)size);
    buffer = new char[size+1];

    file.read(buffer, size); buffer[size] = 0;
    info("file contents:\n\"\"\"\n%s\n\"\"\"", buffer);

    delete[] buffer;
    file.close();

    done(); return true;
}
//------------------------------------------------------------------------------

bool testPathCreation()
{
    start("Path Creation");

    FileWrapper file;

    assert(file.writeAll("testOutput/A/B/C/D/E/file.txt", (const byte*)"Hello World!\n"), return false);

    done(); return true;
}
//------------------------------------------------------------------------------

int main()
{
    setupTerminal();

    printf("\n\n");
    if(!testFileWrapper ()) goto MainError;
    if(!testPathCreation()) goto MainError;

    info(ANSI_FG_GREEN "All OK"); done();
    return 0;

    MainError:
        done(); info(ANSI_FG_BRIGHT_RED "There were errors");
        return -1;
}
//------------------------------------------------------------------------------

