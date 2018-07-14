//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
//
// This file is part of a library
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#ifndef clUnicodeString_h
#define clUnicodeString_h
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//------------------------------------------------------------------------------

#define char16 uint16_t
#define char32 uint32_t
//------------------------------------------------------------------------------

class clUnicodeString{
  private:
    // UTF-32 container (main container)
    char32* Data_32;
    size_t  Length_32;
    size_t  Allocated_32;

    // UTF-8 and UTF-16 containers (filled on demand)
    char*  Data_8;
    bool   Valid_8;
    size_t Length_8;
    size_t Allocated_8;

    char16* Data_16;
    bool    Valid_16;
    size_t  Length_16;
    size_t  Allocated_16;

    void Invalidate();

    void Append(char   Data);
    void Append(char16 Data);
    void Append(char32 Data);

    void Update8 (); // Fills Data_8 from Data_32
    void Update16(); // Fills Data_16 from Data_32

    void Update32_From8 (); // Fills Data_32 from Data_8
    void Update32_From16(); // Fills Data_32 from Data_16

  public:
    clUnicodeString();
   ~clUnicodeString();

    void operator= (char     c);
    void operator= (char16   c);
    void operator= (char32   c);
    void operator= (int      d);
    void operator= ( int64_t d);
    void operator= (uint64_t d);
    void operator= (double   f);

    void operator= (const char  * String); // UTF-8
    void operator= (const char16* String); // UTF-16
    void operator= (const char32* String); // UTF-32
    void operator= (const clUnicodeString& String);

    void operator+= (char     c);
    void operator+= (char16   c);
    void operator+= (char32   c);
    void operator+= (int      d);
    void operator+= ( int64_t d);
    void operator+= (uint64_t d);
    void operator+= (double   f);

    void operator+= (const char  * String); // UTF-8
    void operator+= (const char16* String); // UTF-16
    void operator+= (const char32* String); // UTF-32
    void operator+= (const clUnicodeString& String);

    bool operator== (const char  * String); // UTF-8
    bool operator== (const char16* String); // UTF-16
    bool operator== (const char32* String); // UTF-32
    bool operator== (      clUnicodeString& String);

    bool operator!= (const char  * String); // UTF-8
    bool operator!= (const char16* String); // UTF-16
    bool operator!= (const char32* String); // UTF-32
    bool operator!= (      clUnicodeString& String);

    bool operator< (const char  * String); // UTF-8
    bool operator< (const char16* String); // UTF-16
    bool operator< (const char32* String); // UTF-32
    bool operator< (      clUnicodeString& String);

    bool operator> (const char  * String); // UTF-8
    bool operator> (const char16* String); // UTF-16
    bool operator> (const char32* String); // UTF-32
    bool operator> (      clUnicodeString& String);

    char32 operator[] (size_t Index);

    void   SetLength32(size_t Length);

    size_t Length8 (); // In UTF-8  bytes
    size_t Length16(); // In UTF-16 words
    size_t Length32(); // In UTF-32 characters

    // Memory managed by this class
    char  * UTF8 ();
    char16* UTF16();
    char32* UTF32();

    // Allocates new memory
    char  * New8 ();
    char16* New16();
    char32* New32();

    void SetHex  (unsigned i, int Places);
    void SetFloat(double   d, int SignificantFigures = 9, bool Fill = false);

    void AppendHex  (unsigned i, int Places);
    void AppendFloat(double   d, int SignificantFigures = 9, bool Fill = false);

    void UpperCase(); // Make upper case

    // Returns -1 for smaller, 0 for equal and 1 for larger
    int Compare      (const clUnicodeString& String);
    int CompareNoCase(const clUnicodeString& String);
    int CompareNoCase(const char  * String);
};
//------------------------------------------------------------------------------

bool operator== (const char  * String1, clUnicodeString& String2); // UTF-8
bool operator== (const char16* String1, clUnicodeString& String2); // UTF-16
bool operator== (const char32* String1, clUnicodeString& String2); // UTF-32

bool operator< (const char  * String1, clUnicodeString& String2); // UTF-8
bool operator< (const char16* String1, clUnicodeString& String2); // UTF-16
bool operator< (const char32* String1, clUnicodeString& String2); // UTF-32

bool operator> (const char  * String1, clUnicodeString& String2); // UTF-8
bool operator> (const char16* String1, clUnicodeString& String2); // UTF-16
bool operator> (const char32* String1, clUnicodeString& String2); // UTF-32
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
