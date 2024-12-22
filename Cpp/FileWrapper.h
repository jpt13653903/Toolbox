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

#ifndef FileWrapper_h
#define FileWrapper_h
//------------------------------------------------------------------------------

#include <vector>
//------------------------------------------------------------------------------

#include "General.h"
#include "UTF_Converter.h"
//------------------------------------------------------------------------------

#if   defined(WINVER)
#elif defined(NIX)
#else
    #error FileWrapper is not implemented for your platform
#endif
//------------------------------------------------------------------------------

class FileWrapper{
    public:
        enum class Access{
            Read,
            Write,
            Create
        };
    private:
        #if defined(WINVER)
            HANDLE handle;
        #elif defined(NIX)
            std::string filename;
            File*       handle;
        #endif

        void getLongName(const wchar_t* filename, std::wstring& longName);
        bool createPath (const char* filename);

    public:
        FileWrapper();
       ~FileWrapper();

        bool open (const  char  * filename, Access access); // UTF-8
        bool open (const wchar_t* filename, Access access); // Windows only: UTF-16
        void close();

        uint64_t getSize();

        uint64_t read (      char* buffer, uint64_t mustRead);
        uint64_t write(const char* buffer); // Null-terminated
        uint64_t write(const char* buffer, uint64_t mustWrite);
        bool     flush();

        // Opens, reads and closes the file
        // UTF-8 name; returns null on error; null-terminates the buffer
        // The caller must free the returned buffer with "delete[]"
        byte* readAll(const char* filename, uint64_t* filesize = 0);

        // Opens, writes and closes the file
        // UTF-8 name; also creates the path if it does not exist
        // If Size is 0, Data is assumed to be null-terminated
        bool writeAll(const char* filename, const byte* data, uint64_t size = 0);

        #ifdef WINVER
            void getTime(FILETIME* creation, FILETIME* access, FILETIME* modified);
            void setTime(
                const FILETIME* creation,
                const FILETIME* access,
                const FILETIME* modified
            );
        #else
            void getTime(time_t* statusChange, time_t* access, time_t* modified);
            void setTime(
                const time_t* statusChange,
                const time_t* access,
                const time_t* modified
            );
        #endif
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
