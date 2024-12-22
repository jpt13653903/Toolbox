/*==============================================================================
Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#ifndef JSON_h
#define JSON_h
//------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
//------------------------------------------------------------------------------

#include "General.h"
//------------------------------------------------------------------------------

class Json{
    public: // Object type
        enum Type{
            typeNull  , typeTrue, typeFalse,
            typeString, typeNumber,
            typeObject, typeArray,
        } type;
//------------------------------------------------------------------------------

    public: // Base types
        std::string string;
        double      number;

        // These functions make copies of the value
        void operator= (const char* value);
        void operator= (int         value);
        void operator= (unsigned    value);
        void operator= (double      value);
        void operator= (bool        value);
//------------------------------------------------------------------------------

    public: // Object-related functions
        std::map<std::string, Json*> objects;

        // Adds a new key-value pair, or updates the existing
        // - If "Value" is "typeObject", the update is recursive (i.e. old
        //   values are not deleted, while new ones are added or updated)
        // - These functions make copies of the value.
        // - Returns the JSON object containing the value.
        // - If no "Value" is provided, it either creates a new object with
        //   value "null", or returns the value of the current object.
        // - If no "Name" is provided, the update operation is on the root object
        Json* addOrUpdate(                  Json&       value);
        Json* addOrUpdate(const char* name, Json&       value);
        Json* addOrUpdate(const char* name, const char* value);
        Json* addOrUpdate(const char* name, int         value);
        Json* addOrUpdate(const char* name, unsigned    value);
        Json* addOrUpdate(const char* name, double      value);
        Json* addOrUpdate(const char* name, bool        value);
        Json* addOrUpdate(const char* name);

        // Cannot return a reference, because it could be null (i.e. not in the list)
        Json* operator[] (const char* name);
//------------------------------------------------------------------------------

    public: // Array-related functions
        std::vector<Json*> items;

        // These functions make copies of the value
        // If the type is not "null" and not "Array", the current value becomes
        // the first item of the array and the provided "Value" the second.
        void append(Json&       value);
        void append(const char* value);
        void append(int         value);
        void append(unsigned    value);
        void append(double      value);
        void append(bool        value);

        // Cannot return a reference, because it could be null (i.e. not in the array)
        Json* operator[] (int index);
//------------------------------------------------------------------------------

    private: // Private stuff
        std::string stringification; // Used to return from Stringify()
//------------------------------------------------------------------------------

    private: // Parser
        const char* readBuffer;
        unsigned    readLine;
        unsigned    readSize;
        unsigned    readIndex;

        void readError(const char* message);
        void readSpace();
        void readLineComment();
        void readBlockComment();

        bool readUnicodeSequence(std::string* string);
        bool readIdentifierStart(std::string* string);
        bool readIdentifierPart (std::string* string);

        bool readHexadecimal(Json* value, bool sign);
        bool readIdentifier (std::string* value);
        bool readString     (std::string* value);
        bool readString     (Json* value);
        bool readNumber     (Json* value);
        bool readObject     (Json* objectList);
        bool readArray      (Json* objectList);
        bool readValue      (Json* value);
//------------------------------------------------------------------------------

    public:
        Json();
        Json(Json&       value);
        Json(const char* value);
        Json(int         value);
        Json(unsigned    value);
        Json(double      value);
        Json(bool        value);
       ~Json();

        // If "length" is 0, "strlen" is used to determine the length
        bool parse(const char* json, unsigned length = 0);

        // This function makes a copy of the contents
        void operator=(Json& json);

        // Discard all previous data
        void clear();

        // Converts to a JSON string (internal allocation, do not free)
        const char* stringify();
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
