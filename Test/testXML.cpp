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

bool TestLoad(){
  Start("Testing loading XML files");

  XML xml;
  Assert(xml.Load("Resources/XML.xml"));

  Assert(xml.Save("testOutput/XML.xml"));

  Done(); return true;
}
//------------------------------------------------------------------------------

bool TestBuild(){
  Start("Testing building XML files");

  XML xml;

  xml.New("MyDocument");
    xml.Comment("Comment 1");
    xml.Comment("Comment 2");
    xml.Begin("Entity 1");
      xml.Comment("Comment 2");
      xml.Comment("Comment 3");
      xml.Attribute("Integer" , -12345);
      xml.Attribute("Boolean" , true);
      xml.Attribute("Double"  , 123.456);
      xml.Attribute("Unsigned", 12345);
      xml.Attribute("char_p"  , "Hello World");
    xml.End();
    xml.Begin("Entity 2");
      xml.Begin("Entity 2-1");
        xml.Comment("Comment 4");
        xml.Comment("Comment 5");
        xml.Attribute("Integer" , -12345);
        xml.Attribute("Boolean" , true);
        xml.Attribute("Double"  , 123.456);
        xml.Attribute("Unsigned", 12345);
        xml.Attribute("char_p"  , "Hello World");
      xml.End();
      xml.Begin("Entity 3-1");
        xml.Comment("Comment 6");
        xml.Comment("Comment 7");
        xml.Attribute("Integer" , -12345);
        xml.Attribute("Boolean" , true);
        xml.Attribute("Double"  , 123.456);
        xml.Attribute("Unsigned", 12345);
        xml.Attribute("char_p"  , "Hello World");
      xml.End();
      xml.Comment("Comment 8");
      xml.Comment("Comment 9");
      xml.Attribute("Integer" , -12345);
      xml.Attribute("Boolean" , true);
      xml.Attribute("Double"  , 123.456);
      xml.Attribute("Unsigned", 12345);
      xml.Attribute("char_p"  , "Hello World");
    xml.End();
    xml.Begin("Entity 3");
      xml.Comment("Comment 10");
      xml.Comment("Comment 11");
      xml.Attribute("Integer" , -12345);
      xml.Attribute("Boolean" , true);
      xml.Attribute("Double"  , 123.456);
      xml.Attribute("Unsigned", 12345);
      xml.Attribute("char_p"  , "Hello World");
      xml.Content  (-12345);        xml.Content("\n");
      xml.Content  (true);          xml.Content("\n");
      xml.Content  (123.456);       xml.Content("\n");
      xml.Content  (12345);         xml.Content("\n");
      xml.Content  ("Hello World"); xml.Content("\n");
    xml.End();
  xml.End();

  Assert(xml.Save("testOutput/Build.xml"));
  Assert(xml.Load("testOutput/Build.xml"));

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  SetupTerminal();

  printf("\n\n");
  if(!TestLoad ()) goto main_Error;
  if(!TestBuild()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    fflush(stdout);
    Sleep(100);
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

