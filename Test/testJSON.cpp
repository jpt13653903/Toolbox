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
#include "JSON.h"
#include "FileWrapper.h"
//------------------------------------------------------------------------------

bool TestBuild(){
  Start("Testing json building and access functions");

  JSON json;
  json.AddOrUpdate("String", "MyString");
  json.AddOrUpdate("Number", 123.456);
  json.AddOrUpdate("Wierd" , "\\\"/\b\f\n\r\t and some more...");

  info("json[\"String\"] = %s", json["String"]->Stringify());
  info("json[\"Number\"] = %s", json["Number"]->Stringify());
  info("json[\"Wierd\" ] = %s", json["Wierd" ]->Stringify());
  info("json = %s", json.Stringify());

  *json["Number"] = 789.456;
  info("json[\"Number\"] = %s", json["Number"]->Stringify());

  JSON* MyArray = json.AddOrUpdate("MyArray");
  info("json[\"MyArray\"] = %s", json["MyArray"]->Stringify());
  for(int j = 0; j < 10; j++)    json["MyArray"]->Append(j);
  info("json[\"MyArray\"] = %s", MyArray        ->Stringify());

  info("json = %s", json.Stringify());
  Assert(!strcmp(json.Stringify(),
    "{"
      "\"MyArray\":[0,1,2,3,4,5,6,7,8,9],"
      "\"Number\":789.456,"
      "\"String\":\"MyString\","
      "\"Wierd\":\"\\\\\\\"\\/\\b\\f\\n\\r\\t and some more...\""
    "}"));

  Done(); return true;
}
//------------------------------------------------------------------------------

bool TestLoad(){
  Start("Testing loading from file");

  FILE_WRAPPER File;
  if(!File.Open("Resources/JSON.json", FILE_WRAPPER::faRead)){
    error("Cannot open \"Resources/JSON.json\" for reading");
    return false;
  }

  int   Size   = (int)File.GetSize();
  char* Buffer = new char[Size + 1];
  File.Read(Buffer, Size);
  Buffer[Size] = 0;

  info("Buffer = %s", Buffer);

  JSON json;
  if(!json.Parse(Buffer)){
    error("Parse error");
    return false;
  }
  delete[] Buffer;

  info("json = %s", json.Stringify());
  Assert(!strcmp(json.Stringify(),
    "{"
      "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
      "\"number\":123.456,"
      "\"object\":{"
        "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
        "\"array2\":[],"
        "\"number\":123.456,"
        "\"object\":{},"
        "\"state1\":true,"
        "\"state2\":false,"
        "\"state3\":null,"
        "\"string\":\"String\""
      "},"
      "\"state1\":true,"
      "\"state2\":false,"
      "\"state3\":null,"
      "\"string\":\"String with unicode...Ω...\","
      "\"wierd\":\"\\\\\\\"\\/\\b\\f\\n\\r\\t and some more...\""
    "}"));

  Done();
  Start("Testing recursive AddOrUpdate");

  JSON NewJson;
  NewJson.Parse("{"
                "  \"string\": \"New String\","
                "  \"state4\": true,"
                "  \"array\" : [ 1, 2, 3, 4 ],"
                "  \"object\": {"
                "    \"number\": 987.654,"
                "    \"object\": \"Changed the type to a string\","
                "    \"array2\": [ 8, 7, 6, 5, 4, 3, 2, 1 ]"
                "  }"
                "}");

  info("Updating with: %s", NewJson.Stringify());
  json.AddOrUpdate(NewJson);
  info("After update: %s", json.Stringify());
  Assert(!strcmp(json.Stringify(),
    "{"
      "\"array\":[1,2,3,4],"
      "\"number\":123.456,"
      "\"object\":{"
        "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
        "\"array2\":[8,7,6,5,4,3,2,1],"
        "\"number\":987.654,"
        "\"object\":\"Changed the type to a string\","
        "\"state1\":true,"
        "\"state2\":false,"
        "\"state3\":null,"
        "\"string\":\"String\""
      "},"
      "\"state1\":true,"
      "\"state2\":false,"
      "\"state3\":null,"
      "\"state4\":true,"
      "\"string\":\"New String\","
      "\"wierd\":\"\\\\\\\"\\/\\b\\f\\n\\r\\t and some more...\""
    "}"));

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  SetupTerminal();

  printf("\n\n");
  if(!TestBuild()) goto main_Error;
  if(!TestLoad ()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    fflush(stdout);
    Sleep(100);
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

