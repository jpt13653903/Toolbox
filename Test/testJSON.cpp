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

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  printf("\n\n");
  if(!TestBuild()) goto main_Error;
  if(!TestLoad ()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

