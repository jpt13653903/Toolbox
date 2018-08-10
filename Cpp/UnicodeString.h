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

#ifndef UnicodeString_h
#define UnicodeString_h
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//------------------------------------------------------------------------------

#define char16 uint16_t
#define char32 uint32_t
//------------------------------------------------------------------------------

class UnicodeString{
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
    UnicodeString();
   ~UnicodeString();

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
    void operator= (const UnicodeString& String);

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
    void operator+= (const UnicodeString& String);

    bool operator== (const char  * String); // UTF-8
    bool operator== (const char16* String); // UTF-16
    bool operator== (const char32* String); // UTF-32
    bool operator== (      UnicodeString& String);

    bool operator!= (const char  * String); // UTF-8
    bool operator!= (const char16* String); // UTF-16
    bool operator!= (const char32* String); // UTF-32
    bool operator!= (      UnicodeString& String);

    bool operator< (const char  * String); // UTF-8
    bool operator< (const char16* String); // UTF-16
    bool operator< (const char32* String); // UTF-32
    bool operator< (      UnicodeString& String);

    bool operator> (const char  * String); // UTF-8
    bool operator> (const char16* String); // UTF-16
    bool operator> (const char32* String); // UTF-32
    bool operator> (      UnicodeString& String);

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
    int Compare      (const UnicodeString& String);
    int CompareNoCase(const UnicodeString& String);
    int CompareNoCase(const char  * String);
};
//------------------------------------------------------------------------------

bool operator== (const char  * String1, UnicodeString& String2); // UTF-8
bool operator== (const char16* String1, UnicodeString& String2); // UTF-16
bool operator== (const char32* String1, UnicodeString& String2); // UTF-32

bool operator< (const char  * String1, UnicodeString& String2); // UTF-8
bool operator< (const char16* String1, UnicodeString& String2); // UTF-16
bool operator< (const char32* String1, UnicodeString& String2); // UTF-32

bool operator> (const char  * String1, UnicodeString& String2); // UTF-8
bool operator> (const char16* String1, UnicodeString& String2); // UTF-16
bool operator> (const char32* String1, UnicodeString& String2); // UTF-32
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
