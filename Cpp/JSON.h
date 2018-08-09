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

#ifndef JSON_h
#define JSON_h
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

#include "General.h"
//------------------------------------------------------------------------------

class JSON{
  public: // Object type
    enum TYPE{
      typeNull  , typeTrue, typeFalse,
      typeString, typeNumber,
      typeObject, typeArray,
    } Type;
//------------------------------------------------------------------------------

  public: // Base types
    std::string String;
    double      Number;

    // These functions make copies of the value
    void operator=(const char* Value);
    void operator=(int         Value);
    void operator=(double      Value);
    void operator=(bool        Value);
//------------------------------------------------------------------------------

  public: // Object-related functions
    struct OBJECT{ // Linked list of objects, in order of insertion
      std::string Name;
      JSON*       Value;
      OBJECT*     Next;

      OBJECT(const char* Name);
     ~OBJECT();
    };
    OBJECT* Objects;
    OBJECT* LastObject;

    // These functions make copies of the value
    void AddOrUpdate(const char* Name, JSON&       Value);
    void AddOrUpdate(const char* Name, const char* Value);
    void AddOrUpdate(const char* Name, int         Value);
    void AddOrUpdate(const char* Name, double      Value);
    void AddOrUpdate(const char* Name, bool        Value);
    void AddOrUpdate(const char* Name); // Creates a null object

    // Cannot return a reference, because it could be null (i.e. not in the list)
    JSON* operator[] (const char* Name);
//------------------------------------------------------------------------------

  public: // Array-related functions
    struct ARRAY{ // Linked list of objects, in order of insertion
      JSON*  Value;
      ARRAY* Next;

      ARRAY();
     ~ARRAY();
    };
    ARRAY* Items;
    ARRAY* LastItem;

    // These functions make copies of the value
    // If the type is not "null" and not "Array", the current value becomes
    // the first item and the array and the provided "Value" the second
    void Append(JSON&       Value);
    void Append(const char* Value);
    void Append(int         Value);
    void Append(double      Value);
    void Append(bool        Value);

    // Cannot return a reference, because it could be null (i.e. not in the array)
    JSON* operator[] (int Index);
//------------------------------------------------------------------------------

  private: // Private stuff
    std::string Stringification; // Used to return from Stringify()
//------------------------------------------------------------------------------

  private: // Parser
    const char* ReadBuffer;
    unsigned    ReadLine;
    unsigned    ReadSize;
    unsigned    ReadIndex;

    void ReadError (const char* Message);
    void ReadSpace ();
    bool ReadUnicodeSequence(std::string* String);
    bool ReadString(std::string* Value);
    bool ReadString(JSON* Value);
    bool ReadNumber(JSON* Value);
    bool ReadObject(JSON* ObjectList);
    bool ReadArray (JSON* ObjectList);
    bool ReadValue (JSON* Value);
//------------------------------------------------------------------------------

  public:
    JSON();
    JSON(JSON&       Value);
    JSON(const char* Value);
    JSON(int         Value);
    JSON(double      Value);
    JSON(bool        Value);
   ~JSON();

    bool Parse(const char* json);

    // This function makes a copy of the contents
    void operator=(JSON& json);

    // Discard all previous data
    void Clear();

    // Converts to a JSON string (internal allocation, do not free)
    const char* Stringify();
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
