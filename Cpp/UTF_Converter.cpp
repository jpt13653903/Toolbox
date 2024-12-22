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

// This is done manually because std:wstring_convert throws funny errors
//------------------------------------------------------------------------------

#include "UTF_Converter.h"
//------------------------------------------------------------------------------

UtfConverter utfConverter;
//------------------------------------------------------------------------------

using std::string;
using std::u16string;
using std::u32string;
//------------------------------------------------------------------------------

UtfConverter::UtfConverter()
{
}
//------------------------------------------------------------------------------

UtfConverter::~UtfConverter()
{
}
//------------------------------------------------------------------------------

char32_t UtfConverter::getUtf32(const char* data, int* codeLength)
{
    uint32_t n;
    uint32_t Bits;   // Valid bits
    uint32_t utf32; // The result
    uint8_t  Lead;   // The leading byte
    const uint8_t* s = (const uint8_t*)data;

    if((s[0] & 0xC0) == 0xC0){
        n     = 1;
        Lead  = s[0] << 1;
        Bits  = 0x3F; // 6 bits
        utf32 = s[0];

        while(Lead & 0x80){
            if((s[n] & 0xC0) != 0x80){ // Invalid code-word
                if(codeLength) *codeLength = 1;
                return s[0];
            }
            Bits  = (Bits  << 5) |         0x1F ;
            utf32 = (utf32 << 6) | (s[n] & 0x3F);
            Lead <<= 1;
            n++;
        }
        if(codeLength) *codeLength = n;
        return utf32 & Bits;

    }else{
        if(codeLength) *codeLength = 1;
        return s[0];
    }
}
//------------------------------------------------------------------------------

u32string& UtfConverter::toUtf32(const char* data)
{
    utf32.clear();

    int l;
    int n = 0;
    while(data[n]){
        utf32.append(1, getUtf32(data+n, &l));
        n += l;
    }

    return utf32;
}
//------------------------------------------------------------------------------

u32string& UtfConverter::toUtf32(const char16_t* data)
{
    utf32.clear();

    const uint16_t* u = (const uint16_t*)data;

    char32_t c;

    for(int n = 0; u[n]; n++){
        if(
            ((u[n  ] & 0xFC00) == 0xD800) &&
            ((u[n+1] & 0xFC00) == 0xDC00)
        ){
            c  =              u[n++] & 0x3FF ;
            c  = (c << 10) | (u[n  ] & 0x3FF);
            c += 0x10000;
        }else{
            c = u[n];
        }
        utf32.append(1, c);
    }
    return utf32;
}
//------------------------------------------------------------------------------

u32string& UtfConverter::toUtf32(const string& data)
{
    return toUtf32(data.c_str());
}
//------------------------------------------------------------------------------

u32string& UtfConverter::toUtf32(const u16string& data)
{
    return toUtf32(data.c_str());
}
//------------------------------------------------------------------------------

u16string& UtfConverter::toUtf16(char32_t data)
{
    char32_t s[2] = {data, 0};
    return toUtf16(s);
}
//------------------------------------------------------------------------------

u16string& UtfConverter::toUtf16(const char* data)
{
    return toUtf16(toUtf32(data));
}
//------------------------------------------------------------------------------

u16string& UtfConverter::toUtf16(const char32_t* data)
{
    utf16.clear();

    for(int n = 0; data[n]; n++){
        if(data[n] > 0xFFFF){
            uint32_t c = data[n] - 0x10000;
            utf16.append(1, (char16_t)(0xD800 | (c >>    10)));
            utf16.append(1, (char16_t)(0xDC00 | (c &  0x3FF)));
        }else{
            utf16.append(1, (char16_t)(data[n]));
        }
    }

    return utf16;
}
//------------------------------------------------------------------------------

u16string& UtfConverter::toUtf16(const string& data)
{
    return toUtf16(data.c_str());
}
//------------------------------------------------------------------------------

u16string& UtfConverter::toUtf16(const u32string& data)
{
    return toUtf16(data.c_str());
}
//------------------------------------------------------------------------------

string& UtfConverter::toUtf8(char32_t data)
{
    char32_t s[2] = {data, 0};
    return toUtf8(s);
}
//------------------------------------------------------------------------------

string& UtfConverter::toUtf8(const char16_t* data)
{
    return toUtf8(toUtf32(data));
}
//------------------------------------------------------------------------------

string& UtfConverter::toUtf8(const char32_t* data)
{
    utf8.clear();

    int      n;
    uint32_t c;
    uint8_t  Head;
    uint8_t  Lead;
    uint8_t  Cont[6];

    for(int j = 0; data[j]; j++){
        c    = data[j];
        n    = 0;
        Head = 0x3F; // Active bits in the leading byte
        Lead = 0x80; // Leading byte

        if(c < 0x80){
            utf8.append(1, (char)c);

        }else{
            while(c > Head){ // Doesn't fit in the leading byte
                Cont[n] = 0x80 | (c & 0x3F);
                Lead    = 0x80 | (Lead >> 1);
                Head  >>= 1;
                c     >>= 6;
                n++;
            }
            utf8.append(1, (char)(Lead | c));
            for(n--; n >= 0; n--) utf8.append(1, (char)(Cont[n]));
        }
    }

    return utf8;
}
//------------------------------------------------------------------------------

string& UtfConverter::toUtf8(const u16string& data)
{
    return toUtf8(data.c_str());
}
//------------------------------------------------------------------------------

string& UtfConverter::toUtf8(const u32string& data)
{
    return toUtf8(data.c_str());
}
//------------------------------------------------------------------------------

string& UtfConverter::toUpperCase(const string& data)
{
    return toUtf8(toUpperCase(toUtf32(data)));
}
//------------------------------------------------------------------------------

u16string& UtfConverter::toUpperCase(const u16string& data)
{
    return toUtf16(toUpperCase(toUtf32(data)));
}
//------------------------------------------------------------------------------

u32string& UtfConverter::toUpperCase(const u32string& data)
{
    utf32 = data;

    for(size_t n = 0; utf32[n]; n++){
        if     (utf32[n] >= 0x0061 && utf32[n] <= 0x007A) utf32[n] -= 32;
        else if(utf32[n] >= 0x00E0 && utf32[n] <= 0x00F6) utf32[n] -= 32;
        else if(utf32[n] >= 0x00F8 && utf32[n] <= 0x00FE) utf32[n] -= 32;
        else if(utf32[n] >= 0x023F && utf32[n] <= 0x0240) utf32[n] += 10815;
        else if(utf32[n] >= 0x0256 && utf32[n] <= 0x0257) utf32[n] -= 205;
        else if(utf32[n] >= 0x028A && utf32[n] <= 0x028B) utf32[n] -= 217;
        else if(utf32[n] >= 0x037B && utf32[n] <= 0x037D) utf32[n] += 130;
        else if(utf32[n] >= 0x03AD && utf32[n] <= 0x03AF) utf32[n] -= 37;
        else if(utf32[n] >= 0x03B1 && utf32[n] <= 0x03C1) utf32[n] -= 32;
        else if(utf32[n] >= 0x03C3 && utf32[n] <= 0x03CB) utf32[n] -= 32;
        else if(utf32[n] >= 0x03CD && utf32[n] <= 0x03CE) utf32[n] -= 63;
        else if(utf32[n] >= 0x0430 && utf32[n] <= 0x044F) utf32[n] -= 32;
        else if(utf32[n] >= 0x0450 && utf32[n] <= 0x045F) utf32[n] -= 80;
        else if(utf32[n] >= 0x0561 && utf32[n] <= 0x0586) utf32[n] -= 48;
        else if(utf32[n] >= 0x1F00 && utf32[n] <= 0x1F07) utf32[n] += 8;
        else if(utf32[n] >= 0x1F10 && utf32[n] <= 0x1F15) utf32[n] += 8;
        else if(utf32[n] >= 0x1F20 && utf32[n] <= 0x1F27) utf32[n] += 8;
        else if(utf32[n] >= 0x1F30 && utf32[n] <= 0x1F37) utf32[n] += 8;
        else if(utf32[n] >= 0x1F40 && utf32[n] <= 0x1F45) utf32[n] += 8;
        else if(utf32[n] >= 0x1F60 && utf32[n] <= 0x1F67) utf32[n] += 8;
        else if(utf32[n] >= 0x1F70 && utf32[n] <= 0x1F71) utf32[n] += 74;
        else if(utf32[n] >= 0x1F72 && utf32[n] <= 0x1F75) utf32[n] += 86;
        else if(utf32[n] >= 0x1F76 && utf32[n] <= 0x1F77) utf32[n] += 100;
        else if(utf32[n] >= 0x1F78 && utf32[n] <= 0x1F79) utf32[n] += 128;
        else if(utf32[n] >= 0x1F7A && utf32[n] <= 0x1F7B) utf32[n] += 112;
        else if(utf32[n] >= 0x1F7C && utf32[n] <= 0x1F7D) utf32[n] += 126;
        else if(utf32[n] >= 0x1F80 && utf32[n] <= 0x1F87) utf32[n] += 8;
        else if(utf32[n] >= 0x1F90 && utf32[n] <= 0x1F97) utf32[n] += 8;
        else if(utf32[n] >= 0x1FA0 && utf32[n] <= 0x1FA7) utf32[n] += 8;
        else if(utf32[n] >= 0x1FB0 && utf32[n] <= 0x1FB1) utf32[n] += 8;
        else if(utf32[n] >= 0x1FD0 && utf32[n] <= 0x1FD1) utf32[n] += 8;
        else if(utf32[n] >= 0x1FE0 && utf32[n] <= 0x1FE1) utf32[n] += 8;
        else if(utf32[n] >= 0x2170 && utf32[n] <= 0x217F) utf32[n] -= 16;
        else if(utf32[n] >= 0x24D0 && utf32[n] <= 0x24E9) utf32[n] -= 26;
        else if(utf32[n] >= 0x2C30 && utf32[n] <= 0x2C5E) utf32[n] -= 48;
        else if(utf32[n] >= 0x2D00 && utf32[n] <= 0x2D25) utf32[n] -= 7264;
        else if(utf32[n] >= 0xFF41 && utf32[n] <= 0xFF5A) utf32[n] -= 32;

        else if(utf32[n] == 0x00B5) utf32[n] += 743;
        else if(utf32[n] == 0x00FF) utf32[n] += 121;
        else if(utf32[n] == 0x0101) utf32[n] -= 1;
        else if(utf32[n] == 0x0103) utf32[n] -= 1;
        else if(utf32[n] == 0x0105) utf32[n] -= 1;
        else if(utf32[n] == 0x0107) utf32[n] -= 1;
        else if(utf32[n] == 0x0109) utf32[n] -= 1;
        else if(utf32[n] == 0x010B) utf32[n] -= 1;
        else if(utf32[n] == 0x010D) utf32[n] -= 1;
        else if(utf32[n] == 0x010F) utf32[n] -= 1;
        else if(utf32[n] == 0x0111) utf32[n] -= 1;
        else if(utf32[n] == 0x0113) utf32[n] -= 1;
        else if(utf32[n] == 0x0115) utf32[n] -= 1;
        else if(utf32[n] == 0x0117) utf32[n] -= 1;
        else if(utf32[n] == 0x0119) utf32[n] -= 1;
        else if(utf32[n] == 0x011B) utf32[n] -= 1;
        else if(utf32[n] == 0x011D) utf32[n] -= 1;
        else if(utf32[n] == 0x011F) utf32[n] -= 1;
        else if(utf32[n] == 0x0121) utf32[n] -= 1;
        else if(utf32[n] == 0x0123) utf32[n] -= 1;
        else if(utf32[n] == 0x0125) utf32[n] -= 1;
        else if(utf32[n] == 0x0127) utf32[n] -= 1;
        else if(utf32[n] == 0x0129) utf32[n] -= 1;
        else if(utf32[n] == 0x012B) utf32[n] -= 1;
        else if(utf32[n] == 0x012D) utf32[n] -= 1;
        else if(utf32[n] == 0x012F) utf32[n] -= 1;
        else if(utf32[n] == 0x0131) utf32[n] -= 232;
        else if(utf32[n] == 0x0133) utf32[n] -= 1;
        else if(utf32[n] == 0x0135) utf32[n] -= 1;
        else if(utf32[n] == 0x0137) utf32[n] -= 1;
        else if(utf32[n] == 0x013A) utf32[n] -= 1;
        else if(utf32[n] == 0x013C) utf32[n] -= 1;
        else if(utf32[n] == 0x013E) utf32[n] -= 1;
        else if(utf32[n] == 0x0140) utf32[n] -= 1;
        else if(utf32[n] == 0x0142) utf32[n] -= 1;
        else if(utf32[n] == 0x0144) utf32[n] -= 1;
        else if(utf32[n] == 0x0146) utf32[n] -= 1;
        else if(utf32[n] == 0x0148) utf32[n] -= 1;
        else if(utf32[n] == 0x014B) utf32[n] -= 1;
        else if(utf32[n] == 0x014D) utf32[n] -= 1;
        else if(utf32[n] == 0x014F) utf32[n] -= 1;
        else if(utf32[n] == 0x0151) utf32[n] -= 1;
        else if(utf32[n] == 0x0153) utf32[n] -= 1;
        else if(utf32[n] == 0x0155) utf32[n] -= 1;
        else if(utf32[n] == 0x0157) utf32[n] -= 1;
        else if(utf32[n] == 0x0159) utf32[n] -= 1;
        else if(utf32[n] == 0x015B) utf32[n] -= 1;
        else if(utf32[n] == 0x015D) utf32[n] -= 1;
        else if(utf32[n] == 0x015F) utf32[n] -= 1;
        else if(utf32[n] == 0x0161) utf32[n] -= 1;
        else if(utf32[n] == 0x0163) utf32[n] -= 1;
        else if(utf32[n] == 0x0165) utf32[n] -= 1;
        else if(utf32[n] == 0x0167) utf32[n] -= 1;
        else if(utf32[n] == 0x0169) utf32[n] -= 1;
        else if(utf32[n] == 0x016B) utf32[n] -= 1;
        else if(utf32[n] == 0x016D) utf32[n] -= 1;
        else if(utf32[n] == 0x016F) utf32[n] -= 1;
        else if(utf32[n] == 0x0171) utf32[n] -= 1;
        else if(utf32[n] == 0x0173) utf32[n] -= 1;
        else if(utf32[n] == 0x0175) utf32[n] -= 1;
        else if(utf32[n] == 0x0177) utf32[n] -= 1;
        else if(utf32[n] == 0x017A) utf32[n] -= 1;
        else if(utf32[n] == 0x017C) utf32[n] -= 1;
        else if(utf32[n] == 0x017E) utf32[n] -= 1;
        else if(utf32[n] == 0x017F) utf32[n] -= 300;
        else if(utf32[n] == 0x0180) utf32[n] += 195;
        else if(utf32[n] == 0x0183) utf32[n] -= 1;
        else if(utf32[n] == 0x0185) utf32[n] -= 1;
        else if(utf32[n] == 0x0188) utf32[n] -= 1;
        else if(utf32[n] == 0x018C) utf32[n] -= 1;
        else if(utf32[n] == 0x0192) utf32[n] -= 1;
        else if(utf32[n] == 0x0195) utf32[n] += 97;
        else if(utf32[n] == 0x0199) utf32[n] -= 1;
        else if(utf32[n] == 0x019A) utf32[n] += 163;
        else if(utf32[n] == 0x019E) utf32[n] += 130;
        else if(utf32[n] == 0x01A1) utf32[n] -= 1;
        else if(utf32[n] == 0x01A3) utf32[n] -= 1;
        else if(utf32[n] == 0x01A5) utf32[n] -= 1;
        else if(utf32[n] == 0x01A8) utf32[n] -= 1;
        else if(utf32[n] == 0x01AD) utf32[n] -= 1;
        else if(utf32[n] == 0x01B0) utf32[n] -= 1;
        else if(utf32[n] == 0x01B4) utf32[n] -= 1;
        else if(utf32[n] == 0x01B6) utf32[n] -= 1;
        else if(utf32[n] == 0x01B9) utf32[n] -= 1;
        else if(utf32[n] == 0x01BD) utf32[n] -= 1;
        else if(utf32[n] == 0x01BF) utf32[n] += 56;
        else if(utf32[n] == 0x01C5) utf32[n] -= 1;
        else if(utf32[n] == 0x01C6) utf32[n] -= 2;
        else if(utf32[n] == 0x01C8) utf32[n] -= 1;
        else if(utf32[n] == 0x01C9) utf32[n] -= 2;
        else if(utf32[n] == 0x01CB) utf32[n] -= 1;
        else if(utf32[n] == 0x01CC) utf32[n] -= 2;
        else if(utf32[n] == 0x01CE) utf32[n] -= 1;
        else if(utf32[n] == 0x01D0) utf32[n] -= 1;
        else if(utf32[n] == 0x01D2) utf32[n] -= 1;
        else if(utf32[n] == 0x01D4) utf32[n] -= 1;
        else if(utf32[n] == 0x01D6) utf32[n] -= 1;
        else if(utf32[n] == 0x01D8) utf32[n] -= 1;
        else if(utf32[n] == 0x01DA) utf32[n] -= 1;
        else if(utf32[n] == 0x01DC) utf32[n] -= 1;
        else if(utf32[n] == 0x01DD) utf32[n] -= 79;
        else if(utf32[n] == 0x01DF) utf32[n] -= 1;
        else if(utf32[n] == 0x01E1) utf32[n] -= 1;
        else if(utf32[n] == 0x01E3) utf32[n] -= 1;
        else if(utf32[n] == 0x01E5) utf32[n] -= 1;
        else if(utf32[n] == 0x01E7) utf32[n] -= 1;
        else if(utf32[n] == 0x01E9) utf32[n] -= 1;
        else if(utf32[n] == 0x01EB) utf32[n] -= 1;
        else if(utf32[n] == 0x01ED) utf32[n] -= 1;
        else if(utf32[n] == 0x01EF) utf32[n] -= 1;
        else if(utf32[n] == 0x01F2) utf32[n] -= 1;
        else if(utf32[n] == 0x01F3) utf32[n] -= 2;
        else if(utf32[n] == 0x01F5) utf32[n] -= 1;
        else if(utf32[n] == 0x01F9) utf32[n] -= 1;
        else if(utf32[n] == 0x01FB) utf32[n] -= 1;
        else if(utf32[n] == 0x01FD) utf32[n] -= 1;
        else if(utf32[n] == 0x01FF) utf32[n] -= 1;
        else if(utf32[n] == 0x0201) utf32[n] -= 1;
        else if(utf32[n] == 0x0203) utf32[n] -= 1;
        else if(utf32[n] == 0x0205) utf32[n] -= 1;
        else if(utf32[n] == 0x0207) utf32[n] -= 1;
        else if(utf32[n] == 0x0209) utf32[n] -= 1;
        else if(utf32[n] == 0x020B) utf32[n] -= 1;
        else if(utf32[n] == 0x020D) utf32[n] -= 1;
        else if(utf32[n] == 0x020F) utf32[n] -= 1;
        else if(utf32[n] == 0x0211) utf32[n] -= 1;
        else if(utf32[n] == 0x0213) utf32[n] -= 1;
        else if(utf32[n] == 0x0215) utf32[n] -= 1;
        else if(utf32[n] == 0x0217) utf32[n] -= 1;
        else if(utf32[n] == 0x0219) utf32[n] -= 1;
        else if(utf32[n] == 0x021B) utf32[n] -= 1;
        else if(utf32[n] == 0x021D) utf32[n] -= 1;
        else if(utf32[n] == 0x021F) utf32[n] -= 1;
        else if(utf32[n] == 0x0223) utf32[n] -= 1;
        else if(utf32[n] == 0x0225) utf32[n] -= 1;
        else if(utf32[n] == 0x0227) utf32[n] -= 1;
        else if(utf32[n] == 0x0229) utf32[n] -= 1;
        else if(utf32[n] == 0x022B) utf32[n] -= 1;
        else if(utf32[n] == 0x022D) utf32[n] -= 1;
        else if(utf32[n] == 0x022F) utf32[n] -= 1;
        else if(utf32[n] == 0x0231) utf32[n] -= 1;
        else if(utf32[n] == 0x0233) utf32[n] -= 1;
        else if(utf32[n] == 0x023C) utf32[n] -= 1;
        else if(utf32[n] == 0x0242) utf32[n] -= 1;
        else if(utf32[n] == 0x0247) utf32[n] -= 1;
        else if(utf32[n] == 0x0249) utf32[n] -= 1;
        else if(utf32[n] == 0x024B) utf32[n] -= 1;
        else if(utf32[n] == 0x024D) utf32[n] -= 1;
        else if(utf32[n] == 0x024F) utf32[n] -= 1;
        else if(utf32[n] == 0x0250) utf32[n] += 10783;
        else if(utf32[n] == 0x0251) utf32[n] += 2588;
        else if(utf32[n] == 0x0252) utf32[n] += 10782;
        else if(utf32[n] == 0x0253) utf32[n] -= 210;
        else if(utf32[n] == 0x0254) utf32[n] -= 206;
        else if(utf32[n] == 0x0259) utf32[n] -= 202;
        else if(utf32[n] == 0x025B) utf32[n] -= 203;
        else if(utf32[n] == 0x0260) utf32[n] -= 205;
        else if(utf32[n] == 0x0263) utf32[n] -= 207;
        else if(utf32[n] == 0x0265) utf32[n] += 42280;
        else if(utf32[n] == 0x0268) utf32[n] -= 209;
        else if(utf32[n] == 0x0269) utf32[n] -= 211;
        else if(utf32[n] == 0x026B) utf32[n] += 10743;
        else if(utf32[n] == 0x026F) utf32[n] -= 211;
        else if(utf32[n] == 0x0271) utf32[n] += 10749;
        else if(utf32[n] == 0x0272) utf32[n] -= 213;
        else if(utf32[n] == 0x0275) utf32[n] -= 214;
        else if(utf32[n] == 0x027D) utf32[n] += 10727;
        else if(utf32[n] == 0x0283) utf32[n] -= 218;
        else if(utf32[n] == 0x0288) utf32[n] -= 218;
        else if(utf32[n] == 0x0289) utf32[n] -= 69;
        else if(utf32[n] == 0x028C) utf32[n] -= 71;
        else if(utf32[n] == 0x0292) utf32[n] -= 219;
        else if(utf32[n] == 0x0345) utf32[n] += 84;
        else if(utf32[n] == 0x0371) utf32[n] -= 1;
        else if(utf32[n] == 0x0373) utf32[n] -= 1;
        else if(utf32[n] == 0x0377) utf32[n] -= 1;
        else if(utf32[n] == 0x03AC) utf32[n] -= 38;
        else if(utf32[n] == 0x03C2) utf32[n] -= 31;
        else if(utf32[n] == 0x03CC) utf32[n] -= 64;
        else if(utf32[n] == 0x03D0) utf32[n] -= 62;
        else if(utf32[n] == 0x03D1) utf32[n] -= 57;
        else if(utf32[n] == 0x03D5) utf32[n] -= 47;
        else if(utf32[n] == 0x03D6) utf32[n] -= 54;
        else if(utf32[n] == 0x03D7) utf32[n] -= 8;
        else if(utf32[n] == 0x03D9) utf32[n] -= 1;
        else if(utf32[n] == 0x03DB) utf32[n] -= 1;
        else if(utf32[n] == 0x03DD) utf32[n] -= 1;
        else if(utf32[n] == 0x03DF) utf32[n] -= 1;
        else if(utf32[n] == 0x03E1) utf32[n] -= 1;
        else if(utf32[n] == 0x03E3) utf32[n] -= 1;
        else if(utf32[n] == 0x03E5) utf32[n] -= 1;
        else if(utf32[n] == 0x03E7) utf32[n] -= 1;
        else if(utf32[n] == 0x03E9) utf32[n] -= 1;
        else if(utf32[n] == 0x03EB) utf32[n] -= 1;
        else if(utf32[n] == 0x03ED) utf32[n] -= 1;
        else if(utf32[n] == 0x03EF) utf32[n] -= 1;
        else if(utf32[n] == 0x03F0) utf32[n] -= 86;
        else if(utf32[n] == 0x03F1) utf32[n] -= 80;
        else if(utf32[n] == 0x03F2) utf32[n] += 7;
        else if(utf32[n] == 0x03F5) utf32[n] -= 96;
        else if(utf32[n] == 0x03F8) utf32[n] -= 1;
        else if(utf32[n] == 0x03FB) utf32[n] -= 1;
        else if(utf32[n] == 0x0461) utf32[n] -= 1;
        else if(utf32[n] == 0x0463) utf32[n] -= 1;
        else if(utf32[n] == 0x0465) utf32[n] -= 1;
        else if(utf32[n] == 0x0467) utf32[n] -= 1;
        else if(utf32[n] == 0x0469) utf32[n] -= 1;
        else if(utf32[n] == 0x046B) utf32[n] -= 1;
        else if(utf32[n] == 0x046D) utf32[n] -= 1;
        else if(utf32[n] == 0x046F) utf32[n] -= 1;
        else if(utf32[n] == 0x0471) utf32[n] -= 1;
        else if(utf32[n] == 0x0473) utf32[n] -= 1;
        else if(utf32[n] == 0x0475) utf32[n] -= 1;
        else if(utf32[n] == 0x0477) utf32[n] -= 1;
        else if(utf32[n] == 0x0479) utf32[n] -= 1;
        else if(utf32[n] == 0x047B) utf32[n] -= 1;
        else if(utf32[n] == 0x047D) utf32[n] -= 1;
        else if(utf32[n] == 0x047F) utf32[n] -= 1;
        else if(utf32[n] == 0x0481) utf32[n] -= 1;
        else if(utf32[n] == 0x048B) utf32[n] -= 1;
        else if(utf32[n] == 0x048D) utf32[n] -= 1;
        else if(utf32[n] == 0x048F) utf32[n] -= 1;
        else if(utf32[n] == 0x0491) utf32[n] -= 1;
        else if(utf32[n] == 0x0493) utf32[n] -= 1;
        else if(utf32[n] == 0x0495) utf32[n] -= 1;
        else if(utf32[n] == 0x0497) utf32[n] -= 1;
        else if(utf32[n] == 0x0499) utf32[n] -= 1;
        else if(utf32[n] == 0x049B) utf32[n] -= 1;
        else if(utf32[n] == 0x049D) utf32[n] -= 1;
        else if(utf32[n] == 0x049F) utf32[n] -= 1;
        else if(utf32[n] == 0x04A1) utf32[n] -= 1;
        else if(utf32[n] == 0x04A3) utf32[n] -= 1;
        else if(utf32[n] == 0x04A5) utf32[n] -= 1;
        else if(utf32[n] == 0x04A7) utf32[n] -= 1;
        else if(utf32[n] == 0x04A9) utf32[n] -= 1;
        else if(utf32[n] == 0x04AB) utf32[n] -= 1;
        else if(utf32[n] == 0x04AD) utf32[n] -= 1;
        else if(utf32[n] == 0x04AF) utf32[n] -= 1;
        else if(utf32[n] == 0x04B1) utf32[n] -= 1;
        else if(utf32[n] == 0x04B3) utf32[n] -= 1;
        else if(utf32[n] == 0x04B5) utf32[n] -= 1;
        else if(utf32[n] == 0x04B7) utf32[n] -= 1;
        else if(utf32[n] == 0x04B9) utf32[n] -= 1;
        else if(utf32[n] == 0x04BB) utf32[n] -= 1;
        else if(utf32[n] == 0x04BD) utf32[n] -= 1;
        else if(utf32[n] == 0x04BF) utf32[n] -= 1;
        else if(utf32[n] == 0x04C2) utf32[n] -= 1;
        else if(utf32[n] == 0x04C4) utf32[n] -= 1;
        else if(utf32[n] == 0x04C6) utf32[n] -= 1;
        else if(utf32[n] == 0x04C8) utf32[n] -= 1;
        else if(utf32[n] == 0x04CA) utf32[n] -= 1;
        else if(utf32[n] == 0x04CC) utf32[n] -= 1;
        else if(utf32[n] == 0x04CE) utf32[n] -= 1;
        else if(utf32[n] == 0x04CF) utf32[n] -= 15;
        else if(utf32[n] == 0x04D1) utf32[n] -= 1;
        else if(utf32[n] == 0x04D3) utf32[n] -= 1;
        else if(utf32[n] == 0x04D5) utf32[n] -= 1;
        else if(utf32[n] == 0x04D7) utf32[n] -= 1;
        else if(utf32[n] == 0x04D9) utf32[n] -= 1;
        else if(utf32[n] == 0x04DB) utf32[n] -= 1;
        else if(utf32[n] == 0x04DD) utf32[n] -= 1;
        else if(utf32[n] == 0x04DF) utf32[n] -= 1;
        else if(utf32[n] == 0x04E1) utf32[n] -= 1;
        else if(utf32[n] == 0x04E3) utf32[n] -= 1;
        else if(utf32[n] == 0x04E5) utf32[n] -= 1;
        else if(utf32[n] == 0x04E7) utf32[n] -= 1;
        else if(utf32[n] == 0x04E9) utf32[n] -= 1;
        else if(utf32[n] == 0x04EB) utf32[n] -= 1;
        else if(utf32[n] == 0x04ED) utf32[n] -= 1;
        else if(utf32[n] == 0x04EF) utf32[n] -= 1;
        else if(utf32[n] == 0x04F1) utf32[n] -= 1;
        else if(utf32[n] == 0x04F3) utf32[n] -= 1;
        else if(utf32[n] == 0x04F5) utf32[n] -= 1;
        else if(utf32[n] == 0x04F7) utf32[n] -= 1;
        else if(utf32[n] == 0x04F9) utf32[n] -= 1;
        else if(utf32[n] == 0x04FB) utf32[n] -= 1;
        else if(utf32[n] == 0x04FD) utf32[n] -= 1;
        else if(utf32[n] == 0x04FF) utf32[n] -= 1;
        else if(utf32[n] == 0x0501) utf32[n] -= 1;
        else if(utf32[n] == 0x0503) utf32[n] -= 1;
        else if(utf32[n] == 0x0505) utf32[n] -= 1;
        else if(utf32[n] == 0x0507) utf32[n] -= 1;
        else if(utf32[n] == 0x0509) utf32[n] -= 1;
        else if(utf32[n] == 0x050B) utf32[n] -= 1;
        else if(utf32[n] == 0x050D) utf32[n] -= 1;
        else if(utf32[n] == 0x050F) utf32[n] -= 1;
        else if(utf32[n] == 0x0511) utf32[n] -= 1;
        else if(utf32[n] == 0x0513) utf32[n] -= 1;
        else if(utf32[n] == 0x0515) utf32[n] -= 1;
        else if(utf32[n] == 0x0517) utf32[n] -= 1;
        else if(utf32[n] == 0x0519) utf32[n] -= 1;
        else if(utf32[n] == 0x051B) utf32[n] -= 1;
        else if(utf32[n] == 0x051D) utf32[n] -= 1;
        else if(utf32[n] == 0x051F) utf32[n] -= 1;
        else if(utf32[n] == 0x0521) utf32[n] -= 1;
        else if(utf32[n] == 0x0523) utf32[n] -= 1;
        else if(utf32[n] == 0x0525) utf32[n] -= 1;
        else if(utf32[n] == 0x0527) utf32[n] -= 1;
        else if(utf32[n] == 0x1D79) utf32[n] += 35332;
        else if(utf32[n] == 0x1D7D) utf32[n] += 3814;
        else if(utf32[n] == 0x1E01) utf32[n] -= 1;
        else if(utf32[n] == 0x1E03) utf32[n] -= 1;
        else if(utf32[n] == 0x1E05) utf32[n] -= 1;
        else if(utf32[n] == 0x1E07) utf32[n] -= 1;
        else if(utf32[n] == 0x1E09) utf32[n] -= 1;
        else if(utf32[n] == 0x1E0B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E0D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E0F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E11) utf32[n] -= 1;
        else if(utf32[n] == 0x1E13) utf32[n] -= 1;
        else if(utf32[n] == 0x1E15) utf32[n] -= 1;
        else if(utf32[n] == 0x1E17) utf32[n] -= 1;
        else if(utf32[n] == 0x1E19) utf32[n] -= 1;
        else if(utf32[n] == 0x1E1B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E1D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E1F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E21) utf32[n] -= 1;
        else if(utf32[n] == 0x1E23) utf32[n] -= 1;
        else if(utf32[n] == 0x1E25) utf32[n] -= 1;
        else if(utf32[n] == 0x1E27) utf32[n] -= 1;
        else if(utf32[n] == 0x1E29) utf32[n] -= 1;
        else if(utf32[n] == 0x1E2B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E2D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E2F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E31) utf32[n] -= 1;
        else if(utf32[n] == 0x1E33) utf32[n] -= 1;
        else if(utf32[n] == 0x1E35) utf32[n] -= 1;
        else if(utf32[n] == 0x1E37) utf32[n] -= 1;
        else if(utf32[n] == 0x1E39) utf32[n] -= 1;
        else if(utf32[n] == 0x1E3B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E3D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E3F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E41) utf32[n] -= 1;
        else if(utf32[n] == 0x1E43) utf32[n] -= 1;
        else if(utf32[n] == 0x1E45) utf32[n] -= 1;
        else if(utf32[n] == 0x1E47) utf32[n] -= 1;
        else if(utf32[n] == 0x1E49) utf32[n] -= 1;
        else if(utf32[n] == 0x1E4B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E4D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E4F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E51) utf32[n] -= 1;
        else if(utf32[n] == 0x1E53) utf32[n] -= 1;
        else if(utf32[n] == 0x1E55) utf32[n] -= 1;
        else if(utf32[n] == 0x1E57) utf32[n] -= 1;
        else if(utf32[n] == 0x1E59) utf32[n] -= 1;
        else if(utf32[n] == 0x1E5B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E5D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E5F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E61) utf32[n] -= 1;
        else if(utf32[n] == 0x1E63) utf32[n] -= 1;
        else if(utf32[n] == 0x1E65) utf32[n] -= 1;
        else if(utf32[n] == 0x1E67) utf32[n] -= 1;
        else if(utf32[n] == 0x1E69) utf32[n] -= 1;
        else if(utf32[n] == 0x1E6B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E6D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E6F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E71) utf32[n] -= 1;
        else if(utf32[n] == 0x1E73) utf32[n] -= 1;
        else if(utf32[n] == 0x1E75) utf32[n] -= 1;
        else if(utf32[n] == 0x1E77) utf32[n] -= 1;
        else if(utf32[n] == 0x1E79) utf32[n] -= 1;
        else if(utf32[n] == 0x1E7B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E7D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E7F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E81) utf32[n] -= 1;
        else if(utf32[n] == 0x1E83) utf32[n] -= 1;
        else if(utf32[n] == 0x1E85) utf32[n] -= 1;
        else if(utf32[n] == 0x1E87) utf32[n] -= 1;
        else if(utf32[n] == 0x1E89) utf32[n] -= 1;
        else if(utf32[n] == 0x1E8B) utf32[n] -= 1;
        else if(utf32[n] == 0x1E8D) utf32[n] -= 1;
        else if(utf32[n] == 0x1E8F) utf32[n] -= 1;
        else if(utf32[n] == 0x1E91) utf32[n] -= 1;
        else if(utf32[n] == 0x1E93) utf32[n] -= 1;
        else if(utf32[n] == 0x1E95) utf32[n] -= 1;
        else if(utf32[n] == 0x1E9B) utf32[n] -= 59;
        else if(utf32[n] == 0x1EA1) utf32[n] -= 1;
        else if(utf32[n] == 0x1EA3) utf32[n] -= 1;
        else if(utf32[n] == 0x1EA5) utf32[n] -= 1;
        else if(utf32[n] == 0x1EA7) utf32[n] -= 1;
        else if(utf32[n] == 0x1EA9) utf32[n] -= 1;
        else if(utf32[n] == 0x1EAB) utf32[n] -= 1;
        else if(utf32[n] == 0x1EAD) utf32[n] -= 1;
        else if(utf32[n] == 0x1EAF) utf32[n] -= 1;
        else if(utf32[n] == 0x1EB1) utf32[n] -= 1;
        else if(utf32[n] == 0x1EB3) utf32[n] -= 1;
        else if(utf32[n] == 0x1EB5) utf32[n] -= 1;
        else if(utf32[n] == 0x1EB7) utf32[n] -= 1;
        else if(utf32[n] == 0x1EB9) utf32[n] -= 1;
        else if(utf32[n] == 0x1EBB) utf32[n] -= 1;
        else if(utf32[n] == 0x1EBD) utf32[n] -= 1;
        else if(utf32[n] == 0x1EBF) utf32[n] -= 1;
        else if(utf32[n] == 0x1EC1) utf32[n] -= 1;
        else if(utf32[n] == 0x1EC3) utf32[n] -= 1;
        else if(utf32[n] == 0x1EC5) utf32[n] -= 1;
        else if(utf32[n] == 0x1EC7) utf32[n] -= 1;
        else if(utf32[n] == 0x1EC9) utf32[n] -= 1;
        else if(utf32[n] == 0x1ECB) utf32[n] -= 1;
        else if(utf32[n] == 0x1ECD) utf32[n] -= 1;
        else if(utf32[n] == 0x1ECF) utf32[n] -= 1;
        else if(utf32[n] == 0x1ED1) utf32[n] -= 1;
        else if(utf32[n] == 0x1ED3) utf32[n] -= 1;
        else if(utf32[n] == 0x1ED5) utf32[n] -= 1;
        else if(utf32[n] == 0x1ED7) utf32[n] -= 1;
        else if(utf32[n] == 0x1ED9) utf32[n] -= 1;
        else if(utf32[n] == 0x1EDB) utf32[n] -= 1;
        else if(utf32[n] == 0x1EDD) utf32[n] -= 1;
        else if(utf32[n] == 0x1EDF) utf32[n] -= 1;
        else if(utf32[n] == 0x1EE1) utf32[n] -= 1;
        else if(utf32[n] == 0x1EE3) utf32[n] -= 1;
        else if(utf32[n] == 0x1EE5) utf32[n] -= 1;
        else if(utf32[n] == 0x1EE7) utf32[n] -= 1;
        else if(utf32[n] == 0x1EE9) utf32[n] -= 1;
        else if(utf32[n] == 0x1EEB) utf32[n] -= 1;
        else if(utf32[n] == 0x1EED) utf32[n] -= 1;
        else if(utf32[n] == 0x1EEF) utf32[n] -= 1;
        else if(utf32[n] == 0x1EF1) utf32[n] -= 1;
        else if(utf32[n] == 0x1EF3) utf32[n] -= 1;
        else if(utf32[n] == 0x1EF5) utf32[n] -= 1;
        else if(utf32[n] == 0x1EF7) utf32[n] -= 1;
        else if(utf32[n] == 0x1EF9) utf32[n] -= 1;
        else if(utf32[n] == 0x1EFB) utf32[n] -= 4097;
        else if(utf32[n] == 0x1EFD) utf32[n] -= 1;
        else if(utf32[n] == 0x1EFF) utf32[n] -= 1;
        else if(utf32[n] == 0x1F51) utf32[n] += 8;
        else if(utf32[n] == 0x1F53) utf32[n] += 8;
        else if(utf32[n] == 0x1F55) utf32[n] += 8;
        else if(utf32[n] == 0x1F57) utf32[n] += 8;
        else if(utf32[n] == 0x1FB3) utf32[n] += 9;
        else if(utf32[n] == 0x1FBE) utf32[n] -= 7205;
        else if(utf32[n] == 0x1FC3) utf32[n] += 9;
        else if(utf32[n] == 0x1FE5) utf32[n] += 7;
        else if(utf32[n] == 0x1FF3) utf32[n] += 9;
        else if(utf32[n] == 0x214E) utf32[n] -= 28;
        else if(utf32[n] == 0x2184) utf32[n] -= 1;
        else if(utf32[n] == 0x2C61) utf32[n] -= 1;
        else if(utf32[n] == 0x2C65) utf32[n] -= 10795;
        else if(utf32[n] == 0x2C66) utf32[n] -= 10792;
        else if(utf32[n] == 0x2C68) utf32[n] -= 1;
        else if(utf32[n] == 0x2C6A) utf32[n] -= 1;
        else if(utf32[n] == 0x2C6C) utf32[n] -= 1;
        else if(utf32[n] == 0x2C73) utf32[n] -= 1;
        else if(utf32[n] == 0x2C76) utf32[n] -= 1;
        else if(utf32[n] == 0x2C81) utf32[n] -= 1;
        else if(utf32[n] == 0x2C83) utf32[n] -= 1;
        else if(utf32[n] == 0x2C85) utf32[n] -= 1;
        else if(utf32[n] == 0x2C87) utf32[n] -= 1;
        else if(utf32[n] == 0x2C89) utf32[n] -= 1;
        else if(utf32[n] == 0x2C8B) utf32[n] -= 1;
        else if(utf32[n] == 0x2C8D) utf32[n] -= 1;
        else if(utf32[n] == 0x2C8F) utf32[n] -= 1;
        else if(utf32[n] == 0x2C91) utf32[n] -= 1;
        else if(utf32[n] == 0x2C93) utf32[n] -= 1;
        else if(utf32[n] == 0x2C95) utf32[n] -= 1;
        else if(utf32[n] == 0x2C97) utf32[n] -= 1;
        else if(utf32[n] == 0x2C99) utf32[n] -= 1;
        else if(utf32[n] == 0x2C9B) utf32[n] -= 1;
        else if(utf32[n] == 0x2C9D) utf32[n] -= 1;
        else if(utf32[n] == 0x2C9F) utf32[n] -= 1;
        else if(utf32[n] == 0x2CA1) utf32[n] -= 1;
        else if(utf32[n] == 0x2CA3) utf32[n] -= 1;
        else if(utf32[n] == 0x2CA5) utf32[n] -= 1;
        else if(utf32[n] == 0x2CA7) utf32[n] -= 1;
        else if(utf32[n] == 0x2CA9) utf32[n] -= 1;
        else if(utf32[n] == 0x2CAB) utf32[n] -= 1;
        else if(utf32[n] == 0x2CAD) utf32[n] -= 1;
        else if(utf32[n] == 0x2CAF) utf32[n] -= 1;
        else if(utf32[n] == 0x2CB1) utf32[n] -= 1;
        else if(utf32[n] == 0x2CB3) utf32[n] -= 1;
        else if(utf32[n] == 0x2CB5) utf32[n] -= 1;
        else if(utf32[n] == 0x2CB7) utf32[n] -= 1;
        else if(utf32[n] == 0x2CB9) utf32[n] -= 1;
        else if(utf32[n] == 0x2CBB) utf32[n] -= 1;
        else if(utf32[n] == 0x2CBD) utf32[n] -= 1;
        else if(utf32[n] == 0x2CBF) utf32[n] -= 1;
        else if(utf32[n] == 0x2CC1) utf32[n] -= 1;
        else if(utf32[n] == 0x2CC3) utf32[n] -= 1;
        else if(utf32[n] == 0x2CC5) utf32[n] -= 1;
        else if(utf32[n] == 0x2CC7) utf32[n] -= 1;
        else if(utf32[n] == 0x2CC9) utf32[n] -= 1;
        else if(utf32[n] == 0x2CCB) utf32[n] -= 1;
        else if(utf32[n] == 0x2CCD) utf32[n] -= 1;
        else if(utf32[n] == 0x2CCF) utf32[n] -= 1;
        else if(utf32[n] == 0x2CD1) utf32[n] -= 1;
        else if(utf32[n] == 0x2CD3) utf32[n] -= 1;
        else if(utf32[n] == 0x2CD5) utf32[n] -= 1;
        else if(utf32[n] == 0x2CD7) utf32[n] -= 1;
        else if(utf32[n] == 0x2CD9) utf32[n] -= 1;
        else if(utf32[n] == 0x2CDB) utf32[n] -= 1;
        else if(utf32[n] == 0x2CDD) utf32[n] -= 1;
        else if(utf32[n] == 0x2CDF) utf32[n] -= 1;
        else if(utf32[n] == 0x2CE1) utf32[n] -= 1;
        else if(utf32[n] == 0x2CE3) utf32[n] -= 1;
        else if(utf32[n] == 0x2CEC) utf32[n] -= 1;
        else if(utf32[n] == 0x2CEE) utf32[n] -= 1;
        else if(utf32[n] == 0xA641) utf32[n] -= 1;
        else if(utf32[n] == 0xA643) utf32[n] -= 1;
        else if(utf32[n] == 0xA645) utf32[n] -= 1;
        else if(utf32[n] == 0xA647) utf32[n] -= 1;
        else if(utf32[n] == 0xA649) utf32[n] -= 1;
        else if(utf32[n] == 0xA64B) utf32[n] -= 1;
        else if(utf32[n] == 0xA64D) utf32[n] -= 1;
        else if(utf32[n] == 0xA64F) utf32[n] -= 1;
        else if(utf32[n] == 0xA651) utf32[n] -= 1;
        else if(utf32[n] == 0xA653) utf32[n] -= 1;
        else if(utf32[n] == 0xA655) utf32[n] -= 1;
        else if(utf32[n] == 0xA657) utf32[n] -= 1;
        else if(utf32[n] == 0xA659) utf32[n] -= 1;
        else if(utf32[n] == 0xA65B) utf32[n] -= 1;
        else if(utf32[n] == 0xA65D) utf32[n] -= 1;
        else if(utf32[n] == 0xA65F) utf32[n] -= 1;
        else if(utf32[n] == 0xA661) utf32[n] -= 1;
        else if(utf32[n] == 0xA663) utf32[n] -= 1;
        else if(utf32[n] == 0xA665) utf32[n] -= 1;
        else if(utf32[n] == 0xA667) utf32[n] -= 1;
        else if(utf32[n] == 0xA669) utf32[n] -= 1;
        else if(utf32[n] == 0xA66B) utf32[n] -= 1;
        else if(utf32[n] == 0xA66D) utf32[n] -= 1;
        else if(utf32[n] == 0xA681) utf32[n] -= 1;
        else if(utf32[n] == 0xA683) utf32[n] -= 1;
        else if(utf32[n] == 0xA685) utf32[n] -= 1;
        else if(utf32[n] == 0xA687) utf32[n] -= 1;
        else if(utf32[n] == 0xA689) utf32[n] -= 1;
        else if(utf32[n] == 0xA68B) utf32[n] -= 1;
        else if(utf32[n] == 0xA68D) utf32[n] -= 1;
        else if(utf32[n] == 0xA68F) utf32[n] -= 1;
        else if(utf32[n] == 0xA691) utf32[n] -= 1;
        else if(utf32[n] == 0xA693) utf32[n] -= 1;
        else if(utf32[n] == 0xA695) utf32[n] -= 1;
        else if(utf32[n] == 0xA697) utf32[n] -= 1;
        else if(utf32[n] == 0xA723) utf32[n] -= 1;
        else if(utf32[n] == 0xA725) utf32[n] -= 1;
        else if(utf32[n] == 0xA727) utf32[n] -= 1;
        else if(utf32[n] == 0xA729) utf32[n] -= 1;
        else if(utf32[n] == 0xA72B) utf32[n] -= 1;
        else if(utf32[n] == 0xA72D) utf32[n] -= 1;
        else if(utf32[n] == 0xA72F) utf32[n] -= 1;
        else if(utf32[n] == 0xA733) utf32[n] -= 1;
        else if(utf32[n] == 0xA735) utf32[n] -= 1;
        else if(utf32[n] == 0xA737) utf32[n] -= 1;
        else if(utf32[n] == 0xA739) utf32[n] -= 1;
        else if(utf32[n] == 0xA73B) utf32[n] -= 1;
        else if(utf32[n] == 0xA73D) utf32[n] -= 1;
        else if(utf32[n] == 0xA73F) utf32[n] -= 1;
        else if(utf32[n] == 0xA741) utf32[n] -= 1;
        else if(utf32[n] == 0xA743) utf32[n] -= 1;
        else if(utf32[n] == 0xA745) utf32[n] -= 1;
        else if(utf32[n] == 0xA747) utf32[n] -= 1;
        else if(utf32[n] == 0xA749) utf32[n] -= 1;
        else if(utf32[n] == 0xA74B) utf32[n] -= 1;
        else if(utf32[n] == 0xA74D) utf32[n] -= 1;
        else if(utf32[n] == 0xA74F) utf32[n] -= 1;
        else if(utf32[n] == 0xA751) utf32[n] -= 1;
        else if(utf32[n] == 0xA753) utf32[n] -= 1;
        else if(utf32[n] == 0xA755) utf32[n] -= 1;
        else if(utf32[n] == 0xA757) utf32[n] -= 1;
        else if(utf32[n] == 0xA759) utf32[n] -= 1;
        else if(utf32[n] == 0xA75B) utf32[n] -= 1;
        else if(utf32[n] == 0xA75D) utf32[n] -= 1;
        else if(utf32[n] == 0xA75F) utf32[n] -= 1;
        else if(utf32[n] == 0xA761) utf32[n] -= 1;
        else if(utf32[n] == 0xA763) utf32[n] -= 1;
        else if(utf32[n] == 0xA765) utf32[n] -= 1;
        else if(utf32[n] == 0xA767) utf32[n] -= 1;
        else if(utf32[n] == 0xA769) utf32[n] -= 1;
        else if(utf32[n] == 0xA76B) utf32[n] -= 1;
        else if(utf32[n] == 0xA76D) utf32[n] -= 1;
        else if(utf32[n] == 0xA76F) utf32[n] -= 1;
        else if(utf32[n] == 0xA77A) utf32[n] -= 1;
        else if(utf32[n] == 0xA77C) utf32[n] -= 1;
        else if(utf32[n] == 0xA77F) utf32[n] -= 1;
        else if(utf32[n] == 0xA781) utf32[n] -= 1;
        else if(utf32[n] == 0xA783) utf32[n] -= 1;
        else if(utf32[n] == 0xA785) utf32[n] -= 1;
        else if(utf32[n] == 0xA787) utf32[n] -= 1;
        else if(utf32[n] == 0xA78C) utf32[n] -= 1;
        else if(utf32[n] == 0xA791) utf32[n] -= 1;
        else if(utf32[n] == 0xA7A1) utf32[n] -= 1;
        else if(utf32[n] == 0xA7A3) utf32[n] -= 1;
        else if(utf32[n] == 0xA7A5) utf32[n] -= 1;
        else if(utf32[n] == 0xA7A7) utf32[n] -= 1;
        else if(utf32[n] == 0xA7A9) utf32[n] -= 1;
    }
    return utf32;
}
//------------------------------------------------------------------------------

