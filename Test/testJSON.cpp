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

bool testBuild()
{
    start("Testing json building and access functions");

    Json json;
    json.addOrUpdate("String", "MyString");
    json.addOrUpdate("Number", 123.456);
    json.addOrUpdate("Wierd" , "\\\"/\b\f\n\r\t and some more...");

    info("json[\"String\"] = %s", json["String"]->stringify());
    info("json[\"Number\"] = %s", json["Number"]->stringify());
    info("json[\"Wierd\" ] = %s", json["Wierd" ]->stringify());
    info("json = %s", json.stringify());

    *json["Number"] = 789.456;
    info("json[\"Number\"] = %s", json["Number"]->stringify());

    Json* myArray = json.addOrUpdate("MyArray");
    info("json[\"MyArray\"] = %s", json["MyArray"]->stringify());
    for(int j = 0; j < 10; j++)    json["MyArray"]->append(j);
    info("json[\"MyArray\"] = %s", myArray        ->stringify());

    info("json = %s", json.stringify());
    assert(!strcmp(json.stringify(),
        "{"
            "\"MyArray\":[0,1,2,3,4,5,6,7,8,9],"
            "\"Number\":789.456,"
            "\"String\":\"MyString\","
            "\"Wierd\":\"\\\\\\\"\\/\\b\\f\\n\\r\\t and some more...\""
        "}"), return false);

    done(); return true;
}
//------------------------------------------------------------------------------

bool testLoad()
{
    start("Testing loading from file");

    FileWrapper file;
    if(!file.open("Resources/JSON.json", FileWrapper::Access::Read)){
        error("Cannot open \"Resources/JSON.json\" for reading");
        return false;
    }

    int   size   = (int)file.getSize();
    char* buffer = new char[size + 1];
    file.read(buffer, size);
    buffer[size] = 0;

    info("Buffer = %s", buffer);

    Json json;
    if(!json.parse(buffer)){
        error("Parse error");
        return false;
    }
    delete[] buffer;

    info("json = %s", json.stringify());
    assert(!strcmp(json.stringify(),
        "{"
            "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
            "\"number\":123.456,"
            "\"object\":{"
                "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
                "\"array2\":[],"
                "\"integer\":123,"
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
        "}"), return false);

    done();
    start("Testing recursive addOrUpdate");

    Json newJson;
    newJson.parse("{"
                  "  \"string\": \"New String\","
                  "  \"state4\": true,"
                  "  \"array\" : [ 1, 2, 3, 4 ],"
                  "  \"object\": {"
                  "    \"number\": 987.654,"
                  "    \"object\": \"Changed the type to a string\","
                  "    \"array2\": [ 8, 7, 6, 5, 4, 3, 2, 1 ]"
                  "  }"
                  "}");

    info("Updating with: %s", newJson.stringify());
    json.addOrUpdate(newJson);
    info("After update: %s", json.stringify());
    assert(!strcmp(json.stringify(),
        "{"
            "\"array\":[1,2,3,4],"
            "\"number\":123.456,"
            "\"object\":{"
                "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
                "\"array2\":[8,7,6,5,4,3,2,1],"
                "\"integer\":123,"
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
        "}"), return false);

    done(); return true;
}
//------------------------------------------------------------------------------

bool testJSON5()
{
    start("Testing JSON5 extensions");

    FileWrapper file;
    if(!file.open("Resources/JSON5.json", FileWrapper::Access::Read)){
        error("Cannot open \"Resources/JSON5.json\" for reading");
        return false;
    }

    int   size   = (int)file.getSize();
    char* buffer = new char[size + 1];
    file.read(buffer, size);
    buffer[size] = 0;

    info("Buffer = %s", buffer);

    Json json;
    if(!json.parse(buffer)){
        error("Parse error");
        return false;
    }
    delete[] buffer;

    info("json = %s", json.stringify());
    assert(!strcmp(json.stringify(),
        "{"
            "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
            "\"number\":123.456,"
            "\"object\":{"
                "\"array\":[\"one\",\"two\",\"three\",\"four\"],"
                "\"array2\":[],"
                "\"integer\":123,"
                "\"number2\":912559,"
                "\"number3\":-12648430,"
                "\"object\":{},"
                "\"onlyFractionPart\":0.456,"
                "\"state1\":true,"
                "\"state2\":false,"
                "\"state3\":null,"
                "\"string\":\"String\","
                "\"withExponent\":0.0123,"
                "\"withFractionPart\":-123.456"
            "},"
            "\"state1\":true,"
            "\"state2\":false,"
            "\"state3\":null,"
            "\"string\":\"String with unicode...Ω...\","
            "\"wierd\":\"\\\\\\\"\\/\\b\\f\\n\\r\\t and some more...\""
        "}"), return false);

    done(); return true;
}
//------------------------------------------------------------------------------

int main()
{
    setupTerminal();

    printf("\n\n");
    if(!testBuild()) goto MainError;
    if(!testLoad ()) goto MainError;
    if(!testJSON5()) goto MainError;

    info(ANSI_FG_GREEN "All OK"); done();
    return 0;

    MainError:
        fflush(stdout);
        Sleep(100);
        done(); info(ANSI_FG_BRIGHT_RED "There were errors");
        return -1;
}
//------------------------------------------------------------------------------

