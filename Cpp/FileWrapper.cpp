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

#include "FileWrapper.h"
//------------------------------------------------------------------------------

#ifdef NIX
    #include <utime.h>
#endif
//------------------------------------------------------------------------------

using std::string;
using std::wstring;
using std::vector;
//------------------------------------------------------------------------------

#ifndef WINVER
    #define INVALID_HANDLE_VALUE 0
    #define FILETIME time_t
#endif
//------------------------------------------------------------------------------

FileWrapper::FileWrapper()
{
    handle = INVALID_HANDLE_VALUE;
}
//------------------------------------------------------------------------------

FileWrapper::~FileWrapper()
{
    close();
}
//------------------------------------------------------------------------------

void FileWrapper::getLongName(const wchar_t* filename, wstring& longName)
{
    #ifdef WINVER
        // Extend the Path to use long names
        if(filename[1] == L':' && filename[2] == L'\\') longName = L"\\\\?\\";

        for(int n = 0; filename[n]; n++){
            if(filename[n] == L'/') longName += L'\\';
            else                    longName += filename[n];
        }
    #else
        // Other operating systems don't have this problem
        longName = filename;
    #endif
}
//------------------------------------------------------------------------------

bool FileWrapper::open(const char* filename, Access access)
{
    if(!filename || !filename[0]) return false;

    #ifdef WINVER
        return open((const wchar_t*)utfConverter.toUtf16(filename).c_str(), access);

    #else
        if(handle) close();
        this->filename = filename;

        switch(access){
            case Access::Read:
                handle = fopen(filename, "rb");
                break;

            case Access::Write:
                handle = fopen(filename, "wb");
                if(handle) fseek(handle, 0, SEEK_END);
                break;

            case Access::Create:
                handle = fopen(filename, "wb");
                break;

            default:
                break;
        }
        return handle;
    #endif
}
//------------------------------------------------------------------------------

bool FileWrapper::open(const wchar_t* filename, Access access)
{
    #ifdef WINVER
        if(!filename || !filename[0]) return false;

        if(handle != INVALID_HANDLE_VALUE) close();

        DWORD desiredAccess = 0;
        DWORD shareMode     = 0;
        DWORD creation      = 0;

        switch(access){
            case Access::Read:
                desiredAccess = GENERIC_READ;
                shareMode     = FILE_SHARE_READ;
                creation      = OPEN_EXISTING;
                break;

            case Access::Write:
                desiredAccess = GENERIC_WRITE;
                creation      = OPEN_EXISTING;
                break;

            case Access::Create:
                desiredAccess = GENERIC_WRITE;
                creation      = CREATE_ALWAYS;
                break;

            default:
                break;
        }

        // TODO: Long names (i.e. prefixed with "\\?\" cannot have relative paths.
        //       Relative paths should therefore be resolved if long names are
        //       required.
        // wstring longName;
        // getLongName(filename, longName);

        handle = CreateFileW(
            filename,
            desiredAccess,
            shareMode,
            0,
            creation,
            FILE_ATTRIBUTE_NORMAL,
            0
        );

        // Try again if failed with CREATE_ALWAYS -- This is a Windows 10 bug
        if(handle == INVALID_HANDLE_VALUE && creation == CREATE_ALWAYS){
            handle = CreateFileW(
                filename,
                desiredAccess,
                shareMode,
                0,
                TRUNCATE_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                0
            );
        }

        if(handle == INVALID_HANDLE_VALUE) return false;

        if(access == Access::Write) SetFilePointer(handle, 0, 0, FILE_END);

        return true;

    #else // Not Windows
        return false;
    #endif
}
//------------------------------------------------------------------------------

void FileWrapper::close()
{
    #ifdef WINVER
        if(handle == INVALID_HANDLE_VALUE) return;
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    #else
        if(handle) fclose(handle);
        handle = 0;
    #endif
}
//------------------------------------------------------------------------------

uint64_t FileWrapper::getSize()
{
    if(handle == INVALID_HANDLE_VALUE) return 0;

    #if defined(WINVER)
        uint64_t result;
        DWORD    high = 0;

        result  = GetFileSize(handle, &high);
        result += ((uint64_t)high) << 32;
        return result;

    #elif defined(NIX)
        long int position = ftell(handle);
        fseek(handle, 0, SEEK_END);
        long int size = ftell(handle);
        fseek(handle, position, SEEK_SET);
        return size;
    #endif
}
//------------------------------------------------------------------------------

uint64_t FileWrapper::read(char* buffer, uint64_t mustRead)
{
    if(handle == INVALID_HANDLE_VALUE) return 0;

    #ifdef WINVER
        DWORD length;
        if(!ReadFile(handle, buffer, mustRead, &length, 0)) return 0;
        return length;
    #else
        return fread(buffer, 1, mustRead, handle);
    #endif
}
//------------------------------------------------------------------------------

uint64_t FileWrapper::write(const char* buffer)
{
    int j;
    for(j = 0; buffer[j]; j++);
    return write(buffer, j);
}
//------------------------------------------------------------------------------

uint64_t FileWrapper::write(const char* buffer, uint64_t mustWrite)
{
    if(handle == INVALID_HANDLE_VALUE) return 0;

    #ifdef WINVER
        DWORD length;
        if(!WriteFile(handle, buffer, mustWrite, &length, 0)) return 0;
        return length;
    #else
        return fwrite(buffer, 1, mustWrite, handle);
    #endif
}
//------------------------------------------------------------------------------

bool FileWrapper::flush()
{
    if(handle == INVALID_HANDLE_VALUE) return false;

    #ifdef WINVER
        return FlushFileBuffers(handle);
    #else
        return !fflush(handle);
    #endif
}
//------------------------------------------------------------------------------

byte* FileWrapper::readAll(const char* filename, uint64_t* filesize)
{
    if(!open(filename, Access::Read)) return 0;
    uint64_t size = getSize();
    if(filesize) *filesize = size;

    byte* buffer = new byte[size+1];
    if(!buffer) return 0;

    if(read((char*)buffer, size) != size){
        delete[] buffer;
        close();
        return 0;
    }
    buffer[size] = 0;

    close();
    return buffer;
}
//------------------------------------------------------------------------------

bool FileWrapper::createPath(const char* filename)
{
    #ifdef WINVER
        wstring path;
        getLongName((const wchar_t*)utfConverter.toUtf16(filename).c_str(), path);

        vector<int> slashes;

        int n;
        for(n = 0; path[n]; n++){
            if(path[n] == L'\\') slashes.push_back(n);
        }
        if(slashes.empty()) return true;

        int  s;
        bool exists;

        n = slashes.size()-1;
        while(0 <= n && n < (int)slashes.size()){
            s = slashes[n];
            path[s] = 0;
                exists = CreateDirectory(path.c_str(), 0);
                if(!exists) exists = (GetLastError() == ERROR_ALREADY_EXISTS);
            path[s] = '\\';

            if(exists) n++;
            else       n--;
        }
        return n >= 0;

    #else
        string path = filename;

        vector<int> slashes;

        int n;
        for(n = 0; path[n]; n++){
            if(path[n] == '/') slashes.push_back(n);
        }
        if(slashes.empty()) return true;

        int  s;
        bool exists;

        n = slashes.size()-1;
        while(0 <= n && n < (int)slashes.size()){
            s = slashes[n];
            path[s] = 0;
                exists = (mkdir(path.c_str(), 0777) == 0);
                if(!exists) exists = (errno == EEXIST);
            path[s] = '/';

            if(exists) n++;
            else       n--;
        }
        return n >= 0;
    #endif
}
//------------------------------------------------------------------------------

bool FileWrapper::writeAll(const char* filename, const byte* data, uint64_t size)
{
    if(!filename || !filename[0]) return false;
    if(!createPath(filename)    ) return false;
    if(!open(filename, Access::Create)) return false;

    if(!size) for(size = 0; data[size]; size++);

    bool result = (write((const char*)data, size) == size);

    close();
    return result;
}
//------------------------------------------------------------------------------

void FileWrapper::getTime(
    FILETIME* creation,
    FILETIME* access,
    FILETIME* modified
){
    if(handle == INVALID_HANDLE_VALUE) return;

    #ifdef WINVER
        GetFileTime(handle, creation, access, modified);
    #else
        struct stat statbuf;
        if(!stat(filename.c_str(), &statbuf)){
            *creation = statbuf.st_ctime;
            *access   = statbuf.st_atime;
            *modified = statbuf.st_mtime;
        }
    #endif
}
//------------------------------------------------------------------------------

void FileWrapper::setTime(
    const FILETIME* creation,
    const FILETIME* access,
    const FILETIME* modified
){
    if(handle == INVALID_HANDLE_VALUE) return;

    #ifdef WINVER
        SetFileTime(handle, creation, access, modified);
    #else
        utimbuf time;
        time.actime  = *access;
        time.modtime = *modified;
        utime(filename.c_str(), &time);
    #endif
}
//------------------------------------------------------------------------------
