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

#ifndef UTF_Converter_H
#define UTF_Converter_H
//------------------------------------------------------------------------------

#include <stdint.h>
#include <string>
//------------------------------------------------------------------------------

class UtfConverter{
    private:
        // The conversion functions return references to these objects.  For
        // thread-safety, use multiple instances of this class.
        std::u32string utf32;
        std::u16string utf16;
        std::string    utf8;

        char32_t getUtf32(const char* data, int* codeLength);

    public:
        UtfConverter();
       ~UtfConverter();

        std::u32string& toUtf32    (const char*           data);
        std::u32string& toUtf32    (const char16_t*       data);
        std::u32string& toUtf32    (const std::string&    data);
        std::u32string& toUtf32    (const std::u16string& data);

        std::u16string& toUtf16    (const char32_t        data);
        std::u16string& toUtf16    (const char*           data);
        std::u16string& toUtf16    (const char32_t*       data);
        std::u16string& toUtf16    (const std::string&    data);
        std::u16string& toUtf16    (const std::u32string& data);

        std::string&    toUtf8     (const char32_t        data);
        std::string&    toUtf8     (const char16_t*       data);
        std::string&    toUtf8     (const char32_t*       data);
        std::string&    toUtf8     (const std::u16string& data);
        std::string&    toUtf8     (const std::u32string& data);

        std::string&    toUpperCase(const std::string&    data);
        std::u16string& toUpperCase(const std::u16string& data);
        std::u32string& toUpperCase(const std::u32string& data);
};
//------------------------------------------------------------------------------

extern UtfConverter utfConverter;
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

