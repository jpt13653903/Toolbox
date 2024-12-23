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
#include "UTF_Converter.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

char utf8[] = "Hello World!; "
              "Γειά σου Κόσμε!; "
              "สวัสดีชาวโลก!; "
              "你好，世界; "
              "こんにちは世界！"
              "😀😁😂😃😄😅😆😇😈😉😊😋😌😍😎😏"
              "😐😑😒😓😔😕😖😗😘😙😚😛😜😝😞😟"
              "😠😡😢😣😤😥😦😧😨😩😪😫😬😭😮😯"
              "😰😱😲😳😴😵😶😷😸😹😺😻😼😽😾😿"
              "🙀🙁🙂🙃🙄🙅🙆🙇🙈🙉🙊🙋🙌🙍🙎🙏"
              "\n";

// Converted using https://onlineutf8tools.com/convert-utf8-to-utf16
char16_t utf16[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20,
    0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x3B, 0x20,
    0x393, 0x3B5, 0x3B9, 0x3AC, 0x20,
    0x3C3, 0x3BF, 0x3C5, 0x20,
    0x39A, 0x3CC, 0x3C3, 0x3BC, 0x3B5, 0x21, 0x3B, 0x20,
    0xE2A, 0xE27, 0xE31, 0xE2A, 0xE14, 0xE35, 0xE0A,
    0xE32, 0xE27, 0xE42, 0xE25, 0xE01, 0x21, 0x3B, 0x20,
    0x4F60, 0x597D, 0xFF0C, 0x4E16, 0x754C, 0x3B, 0x20,
    0x3053, 0x3093, 0x306B, 0x3061, 0x306F, 0x4E16, 0x754C, 0xFF01,
    0xD83D, 0xDE00, 0xD83D, 0xDE01, 0xD83D, 0xDE02, 0xD83D, 0xDE03, 0xD83D, 0xDE04,
    0xD83D, 0xDE05, 0xD83D, 0xDE06, 0xD83D, 0xDE07, 0xD83D, 0xDE08, 0xD83D, 0xDE09,
    0xD83D, 0xDE0A, 0xD83D, 0xDE0B, 0xD83D, 0xDE0C, 0xD83D, 0xDE0D, 0xD83D, 0xDE0E,
    0xD83D, 0xDE0F, 0xD83D, 0xDE10, 0xD83D, 0xDE11, 0xD83D, 0xDE12, 0xD83D, 0xDE13,
    0xD83D, 0xDE14, 0xD83D, 0xDE15, 0xD83D, 0xDE16, 0xD83D, 0xDE17, 0xD83D, 0xDE18,
    0xD83D, 0xDE19, 0xD83D, 0xDE1A, 0xD83D, 0xDE1B, 0xD83D, 0xDE1C, 0xD83D, 0xDE1D,
    0xD83D, 0xDE1E, 0xD83D, 0xDE1F, 0xD83D, 0xDE20, 0xD83D, 0xDE21, 0xD83D, 0xDE22,
    0xD83D, 0xDE23, 0xD83D, 0xDE24, 0xD83D, 0xDE25, 0xD83D, 0xDE26, 0xD83D, 0xDE27,
    0xD83D, 0xDE28, 0xD83D, 0xDE29, 0xD83D, 0xDE2A, 0xD83D, 0xDE2B, 0xD83D, 0xDE2C,
    0xD83D, 0xDE2D, 0xD83D, 0xDE2E, 0xD83D, 0xDE2F, 0xD83D, 0xDE30, 0xD83D, 0xDE31,
    0xD83D, 0xDE32, 0xD83D, 0xDE33, 0xD83D, 0xDE34, 0xD83D, 0xDE35, 0xD83D, 0xDE36,
    0xD83D, 0xDE37, 0xD83D, 0xDE38, 0xD83D, 0xDE39, 0xD83D, 0xDE3A, 0xD83D, 0xDE3B,
    0xD83D, 0xDE3C, 0xD83D, 0xDE3D, 0xD83D, 0xDE3E, 0xD83D, 0xDE3F, 0xD83D, 0xDE40,
    0xD83D, 0xDE41, 0xD83D, 0xDE42, 0xD83D, 0xDE43, 0xD83D, 0xDE44, 0xD83D, 0xDE45,
    0xD83D, 0xDE46, 0xD83D, 0xDE47, 0xD83D, 0xDE48, 0xD83D, 0xDE49, 0xD83D, 0xDE4A,
    0xD83D, 0xDE4B, 0xD83D, 0xDE4C, 0xD83D, 0xDE4D, 0xD83D, 0xDE4E, 0xD83D, 0xDE4F,
    0x0A,
    0
};

// Converted using https://onlineutf8tools.com/convert-utf8-to-utf32
char32_t utf32[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20,
    0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x3B, 0x20,
    0x393, 0x3B5, 0x3B9, 0x3AC, 0x20,
    0x3C3, 0x3BF, 0x3C5, 0x20,
    0x39A, 0x3CC, 0x3C3, 0x3BC, 0x3B5, 0x21, 0x3B, 0x20,
    0xE2A, 0xE27, 0xE31, 0xE2A, 0xE14, 0xE35, 0xE0A,
    0xE32, 0xE27, 0xE42, 0xE25, 0xE01, 0x21, 0x3B, 0x20,
    0x4F60, 0x597D, 0xFF0C, 0x4E16, 0x754C, 0x3B, 0x20,
    0x3053, 0x3093, 0x306B, 0x3061, 0x306F, 0x4E16, 0x754C, 0xFF01,
    0x1F600, 0x1F601, 0x1F602, 0x1F603, 0x1F604, 0x1F605, 0x1F606, 0x1F607,
    0x1F608, 0x1F609, 0x1F60A, 0x1F60B, 0x1F60C, 0x1F60D, 0x1F60E, 0x1F60F,
    0x1F610, 0x1F611, 0x1F612, 0x1F613, 0x1F614, 0x1F615, 0x1F616, 0x1F617,
    0x1F618, 0x1F619, 0x1F61A, 0x1F61B, 0x1F61C, 0x1F61D, 0x1F61E, 0x1F61F,
    0x1F620, 0x1F621, 0x1F622, 0x1F623, 0x1F624, 0x1F625, 0x1F626, 0x1F627,
    0x1F628, 0x1F629, 0x1F62A, 0x1F62B, 0x1F62C, 0x1F62D, 0x1F62E, 0x1F62F,
    0x1F630, 0x1F631, 0x1F632, 0x1F633, 0x1F634, 0x1F635, 0x1F636, 0x1F637,
    0x1F638, 0x1F639, 0x1F63A, 0x1F63B, 0x1F63C, 0x1F63D, 0x1F63E, 0x1F63F,
    0x1F640, 0x1F641, 0x1F642, 0x1F643, 0x1F644, 0x1F645, 0x1F646, 0x1F647,
    0x1F648, 0x1F649, 0x1F64A, 0x1F64B, 0x1F64C, 0x1F64D, 0x1F64E, 0x1F64F,
    0x0A,
    0
};
//------------------------------------------------------------------------------

bool testUTF8_to_UTF32()
{
    start("Testing UTF8 -> UTF32");

    u32string result = utfConverter.toUtf32(utf8);

    for(int n = 0; result[n]; n++){
        if(result[n] == utf32[n]){
            info ("result[%02d] = 0x%08X", n, result[n]);
        }else{
            error("result[%02d] = 0x%08X (0x%08X expected)", n, result[n], utf32[n]);
        }
    }
    assert(result == utf32, return false);

    done(); return true;
}
//------------------------------------------------------------------------------

bool testUTF16_to_UTF32()
{
    start("Testing UTF16 -> UTF32");

    u32string result = utfConverter.toUtf32(utf16);

    for(int n = 0; result[n]; n++){
        if(result[n] == utf32[n]){
            info ("result[%02d] = 0x%08X", n, result[n]);
        }else{
            error("result[%02d] = 0x%08X (0x%08X expected)", n, result[n], utf32[n]);
        }
    }
    assert(result == utf32, return false);

    done(); return true;
}
//------------------------------------------------------------------------------

bool testUTF32_to_UTF8()
{
    start("Testing UTF32 -> UTF8");

    string result = utfConverter.toUtf8(utf32);

    for(int n = 0; result[n]; n++){
        if(result[n] == utf8[n]){
            info("result[%02d] = 0x%02X", n, (uint32_t)((unsigned char)result[n]));
        }else{
            error("result[%02d] = 0x%02X (0x%02X expected)", n,
                        (uint32_t)((unsigned char)result[n]),
                        (uint32_t)((unsigned char)utf32[n]));
        }
    }
    assert(result == utf8, return false);

    done(); return true;
}
//------------------------------------------------------------------------------

bool testUTF32_to_UTF16()
{
    start("Testing UTF32 -> UTF16");

    u16string result = utfConverter.toUtf16(utf32);

    for(int n = 0; result[n]; n++){
        if(result[n] == utf16[n]){
            info("result[%02d] = 0x%04X", n, (uint32_t)result[n]);
        }else{
            error("result[%02d] = 0x%04X (0x%04X expected)", n,
                        (uint32_t)result[n], (uint32_t)utf32[n]);
        }
    }
    assert(result == utf16, return false);

    done(); return true;
}
//------------------------------------------------------------------------------

int main()
{
    setupTerminal();

    printf("\n\n");
    if(!testUTF8_to_UTF32 ()) goto MainError;
    if(!testUTF16_to_UTF32()) goto MainError;
    if(!testUTF32_to_UTF8 ()) goto MainError;
    if(!testUTF32_to_UTF16()) goto MainError;

    info(ANSI_FG_GREEN "All OK"); done();
    return 0;

    MainError:
        fflush(stdout);
        Sleep(100);
        done(); info(ANSI_FG_BRIGHT_RED "There were errors");
        return -1;
}
//------------------------------------------------------------------------------

