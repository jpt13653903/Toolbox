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
#include "XML.h"
//------------------------------------------------------------------------------

bool testLoad()
{
    start("Testing loading XML files");

    Xml xml;
    assert(xml.load("Resources/XML.xml"), return false);

    assert(xml.save("testOutput/XML.xml"), return false);

    done(); return true;
}
//------------------------------------------------------------------------------

bool testBuild()
{
    start("Testing building XML files");

    Xml xml;

    xml.newDocument("MyDocument");
        xml.comment("Comment 1");
        xml.comment("Comment 2");
        xml.begin("Entity 1");
            xml.comment("Comment 2");
            xml.comment("Comment 3");
            xml.attribute("Integer" , -12345);
            xml.attribute("Boolean" , true);
            xml.attribute("Double"  , 123.456);
            xml.attribute("Unsigned", 12345);
            xml.attribute("char_p"  , "Hello World");
        xml.end();
        xml.begin("Entity 2");
            xml.begin("Entity 2-1");
                xml.comment("Comment 4");
                xml.comment("Comment 5");
                xml.attribute("Integer" , -12345);
                xml.attribute("Boolean" , true);
                xml.attribute("Double"  , 123.456);
                xml.attribute("Unsigned", 12345);
                xml.attribute("char_p"  , "Hello World");
            xml.end();
            xml.begin("Entity 3-1");
                xml.comment("Comment 6");
                xml.comment("Comment 7");
                xml.attribute("Integer" , -12345);
                xml.attribute("Boolean" , true);
                xml.attribute("Double"  , 123.456);
                xml.attribute("Unsigned", 12345);
                xml.attribute("char_p"  , "Hello World");
            xml.end();
            xml.comment("Comment 8");
            xml.comment("Comment 9");
            xml.attribute("Integer" , -12345);
            xml.attribute("Boolean" , true);
            xml.attribute("Double"  , 123.456);
            xml.attribute("Unsigned", 12345);
            xml.attribute("char_p"  , "Hello World");
        xml.end();
        xml.begin("Entity 3");
            xml.comment("Comment 10");
            xml.comment("Comment 11");
            xml.attribute("Integer" , -12345);
            xml.attribute("Boolean" , true);
            xml.attribute("Double"  , 123.456);
            xml.attribute("Unsigned", 12345);
            xml.attribute("char_p"  , "Hello World");
            xml.content  (-12345);        xml.content("\n");
            xml.content  (true);          xml.content("\n");
            xml.content  (123.456);       xml.content("\n");
            xml.content  (12345);         xml.content("\n");
            xml.content  ("Hello World"); xml.content("\n");
        xml.end();
    xml.end();

    assert(xml.save("testOutput/Build.xml"), return false);
    assert(xml.load("testOutput/Build.xml"), return false);

    done(); return true;
}
//------------------------------------------------------------------------------

int main()
{
    setupTerminal();

    printf("\n\n");
    if(!testLoad ()) goto MainError;
    if(!testBuild()) goto MainError;

    info(ANSI_FG_GREEN "All OK"); done();
    return 0;

    MainError:
        fflush(stdout);
        Sleep(100);
        done(); info(ANSI_FG_BRIGHT_RED "There were errors");
        return -1;
}
//------------------------------------------------------------------------------

