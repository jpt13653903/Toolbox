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

#include "JSON.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

bool operator== (string& A, const char* B){ return !(A.compare(B)); }
//------------------------------------------------------------------------------

// Converts UTF-32 to UTF-8 and appends it to the string
void operator+= (string& S, uint32_t Char){
  int  j = 0;
  byte Head = 0x3F; // Active bits in the leading byte
  byte Lead = 0x80; // Leading byte
  byte Cont[6];     // Continuation bytes

  if(Char < 0x80){
    S += (char)Char;

  }else{
    while(Char > Head){ // Doesn't fit in the leading byte
      Cont[j] = 0x80 | (Char & 0x3F);
      Lead    = 0x80 | (Lead >> 1);
      Head  >>= 1;
      Char  >>= 6;
      j++;
    }
    S += (char)(Lead | Char);
    for(j--; j >= 0; j--) S += (char)(Cont[j]);
  }
}
void operator+= (string& S, uint16_t Char){ S += (uint32_t)Char; }
//------------------------------------------------------------------------------

JSON::OBJECT::OBJECT(const char* Name){
  this->Name  = Name;
  this->Value = new JSON;
  this->Next  = 0;
}
//------------------------------------------------------------------------------

JSON::OBJECT::~OBJECT(){
  delete Value;
}
//------------------------------------------------------------------------------

JSON::ARRAY::ARRAY(){
  this->Value = new JSON;
  this->Next  = 0;
}
//------------------------------------------------------------------------------

JSON::ARRAY::~ARRAY(){
  delete Value;
}
//------------------------------------------------------------------------------

JSON::JSON(){
  Type       = typeNull;
  String     = "";
  Number     = 0;
  Objects    = 0;
  LastObject = 0;
  Items      = 0;
  LastItem   = 0;
}
//------------------------------------------------------------------------------

JSON::JSON(JSON& Value){
  Type       = typeNull;
  String     = "";
  Number     = 0;
  Objects    = 0;
  LastObject = 0;
  Items      = 0;
  LastItem   = 0;
  operator=(Value);
}
//------------------------------------------------------------------------------

JSON::JSON(const char* Value){
  Type       = typeNull;
  String     = "";
  Number     = 0;
  Objects    = 0;
  LastObject = 0;
  Items      = 0;
  LastItem   = 0;
  operator=(Value);
}
//------------------------------------------------------------------------------

JSON::JSON(int Value){
  Type       = typeNull;
  String     = "";
  Number     = 0;
  Objects    = 0;
  LastObject = 0;
  Items      = 0;
  LastItem   = 0;
  operator=(Value);
}
//------------------------------------------------------------------------------

JSON::JSON(double Value){
  Type       = typeNull;
  String     = "";
  Number     = 0;
  Objects    = 0;
  LastObject = 0;
  Items      = 0;
  LastItem   = 0;
  operator=(Value);
}
//------------------------------------------------------------------------------

JSON::JSON(bool Value){
  Type       = typeNull;
  String     = "";
  Number     = 0;
  Objects    = 0;
  LastObject = 0;
  Items      = 0;
  LastItem   = 0;
  operator=(Value);
}
//------------------------------------------------------------------------------

JSON::~JSON(){
  Clear();
}
//------------------------------------------------------------------------------

void JSON::Clear(){
  String = "";
  Number = 0;

  OBJECT* Object;

  while(Objects){
    Object  = Objects;
    Objects = Objects->Next;
    delete Object;
  }
  LastObject = 0;

  ARRAY* Item;

  while(Items){
    Item  = Items;
    Items = Items->Next;
    delete Item;
  }
  LastItem = 0;

  Type = typeNull;
}
//------------------------------------------------------------------------------

void JSON::operator=(JSON& Value){
  Clear();
  Type = Value.Type;

  OBJECT* Object;
  ARRAY * Item;

  switch(Value.Type){
    case typeNull:
    case typeTrue:
    case typeFalse:
      break;

    case typeString:
      String = Value.String;
      break;

    case typeNumber:
      Number = Value.Number;
      break;

    case typeObject:
      Object = Value.Objects;
      while(Object){
        AddOrUpdate(Object->Name.c_str(), *(Object->Value));
        Object = Object->Next;
      }
      break;

    case typeArray:
      Item = Value.Items;
      while(Item){
        Append(*(Item->Value));
        Item = Item->Next;
      }
      break;

    default:
      break;
  }
}
//------------------------------------------------------------------------------

void JSON::operator=(const char* Value){
  Clear();
  Type   = typeString;
  String = Value;
}
//------------------------------------------------------------------------------

void JSON::operator=(int Value){
  Clear();
  Type   = typeNumber;
  Number = Value;
}
//------------------------------------------------------------------------------

void JSON::operator=(double Value){
  Clear();
  Type   = typeNumber;
  Number = Value;
}
//------------------------------------------------------------------------------

void JSON::operator=(bool Value){
  Clear();
  if(Value) Type = typeTrue;
  else      Type = typeFalse;
}
//------------------------------------------------------------------------------

void JSON::AddOrUpdate(const char* Name, JSON& Value){
  Type = typeObject;

  JSON* json = operator[](Name);
  if(json){
    json->operator=(Value);
    return;
  }
  OBJECT* Object = new OBJECT(Name);
  Object->Value->operator=(Value);
  if(LastObject) LastObject->Next = Object;
  else           Objects          = Object;
  LastObject = Object;
}
//------------------------------------------------------------------------------

void JSON::AddOrUpdate(const char* Name, const char* Value){
  JSON json(Value); AddOrUpdate(Name, json);
}
//------------------------------------------------------------------------------

void JSON::AddOrUpdate(const char* Name, int Value){
  JSON json(Value); AddOrUpdate(Name, json);
}
//------------------------------------------------------------------------------

void JSON::AddOrUpdate(const char* Name, double Value){
  JSON json(Value); AddOrUpdate(Name, json);
}
//------------------------------------------------------------------------------

void JSON::AddOrUpdate(const char* Name, bool Value){
  JSON json(Value); AddOrUpdate(Name, json);
}
//------------------------------------------------------------------------------

void JSON::AddOrUpdate(const char* Name){
  JSON json; AddOrUpdate(Name, json);
}
//------------------------------------------------------------------------------

JSON* JSON::operator[] (const char* Name){
  OBJECT* Object = Objects;
  while(Object){
    if(Object->Name == Name) return Object->Value;
    Object = Object->Next;
  }
  return 0;
}
//------------------------------------------------------------------------------

void JSON::Append(JSON& Value){
  // If the array is empty, add any existing object as the first item in the array
  if(Items == 0 && Type != typeNull && Type != typeArray){
    ARRAY* Item = new ARRAY();
    Item->Value->operator=(*this);
    Items    = Item;
    LastItem = Item;
  }
  Type = typeArray;

  ARRAY* Item = new ARRAY();
  Item->Value->operator=(Value);
  if(LastItem) LastItem->Next = Item;
  else         Items          = Item;
  LastItem = Item;
}
//------------------------------------------------------------------------------

void JSON::Append(const char* Value){
  JSON json(Value); Append(json);
}
//------------------------------------------------------------------------------

void JSON::Append(int Value){
  JSON json(Value); Append(json);
}
//------------------------------------------------------------------------------

void JSON::Append(double Value){
  JSON json(Value); Append(json);
}
//------------------------------------------------------------------------------

void JSON::Append(bool Value){
  JSON json(Value); Append(json);
}
//------------------------------------------------------------------------------

JSON* JSON::operator[] (int Index){
  ARRAY* Item = Items;
  while(Item){
    if(!Index) return Item->Value;
    Index--;
    Item = Item->Next;
  }
  return 0;
}
//------------------------------------------------------------------------------

void JSON::ReadError(const char* Message){
  static bool FirstError = true;
  if(FirstError) error("JSON error on line %u:\n  %s", ReadLine, Message);
  FirstError = false;
}
//------------------------------------------------------------------------------

void JSON::ReadSpace(){
  while(ReadIndex < ReadSize){
    switch(ReadBuffer[ReadIndex]){
      case '\n': ReadLine++;
      case '\r':
      case '\t':
      case '\v':
      case ' ' : break;
      default  : return;
    }
    ReadIndex++;
  }
}
//------------------------------------------------------------------------------

bool JSON::ReadUnicodeSequence(std::string* String){
  uint16_t Char = 0;

  for(int n = 0; n < 4; n++){
    ReadIndex++;
    if(ReadIndex >= ReadSize) return false;

    if      (ReadBuffer[ReadIndex] >= '0' && ReadBuffer[ReadIndex] <= '9'){
      Char = 0x10*Char + (ReadBuffer[ReadIndex] - '0');
    }else if(ReadBuffer[ReadIndex] >= 'a' && ReadBuffer[ReadIndex] <= 'f'){
      Char = 0x10*Char + (ReadBuffer[ReadIndex] - 'a' + 0xA);
    }else if(ReadBuffer[ReadIndex] >= 'A' && ReadBuffer[ReadIndex] <= 'F'){
      Char = 0x10*Char + (ReadBuffer[ReadIndex] - 'A' + 0xA);
    }else{
      return false;
    }
  }
  *String += Char;
  return true;
}
//------------------------------------------------------------------------------

bool JSON::ReadString(string* Value){
  ReadSpace();
  if(ReadIndex >= ReadSize) return false;

  if(ReadBuffer[ReadIndex] != '"') return false;
  ReadIndex++;

  *Value = "";

  while(ReadIndex < ReadSize){
    switch(ReadBuffer[ReadIndex]){
      case '"':
        ReadIndex++;
        return true;

      case '\\':
        ReadIndex++;
        if(ReadIndex >= ReadSize){
          ReadError("Incomplete escape sequence");
          return false;
        }
        switch(ReadBuffer[ReadIndex]){
          case '"' : *Value += '"' ; break;
          case '\\': *Value += '\\'; break;
          case '/' : *Value += '/' ; break;
          case 'b' : *Value += '\b'; break;
          case 'f' : *Value += '\f'; break;
          case 'n' : *Value += '\n'; break;
          case 'r' : *Value += '\r'; break;
          case 't' : *Value += '\t'; break;
          case 'u' :
            if(!ReadUnicodeSequence(Value)){
              ReadError("Incomplete Unicode escape sequence");
              return false;
            }
            break;
            
          default:
            ReadError("Unrecognised escape sequence");
            return false;
        }
        break;

      case '\b':
      case '\f':
      case '\n':
      case '\r':
        ReadError("Unexpected control character in string");
        return false;

      default:
        *Value += ReadBuffer[ReadIndex];
        break;
    }
    ReadIndex++;
  }
  ReadError("Incomplete string");
  return false;
}
//------------------------------------------------------------------------------

bool JSON::ReadString(JSON* Value){
  if(ReadString(&(Value->String))){
    Value->Type = typeString;
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------

bool JSON::ReadNumber(JSON* Value){
  ReadSpace();
  if(ReadIndex >= ReadSize) return false;

  bool Sign         = false;
  bool ExponentSign = false;

  if(ReadBuffer[ReadIndex] == '-'){
    Sign = true;
    ReadIndex++;
    if(ReadIndex >= ReadSize) return false;    
  }

  if(ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9'){
    if(Sign) ReadError("Incomplete number");
    return false;
  }

  *Value = 0;

  double Result   = 0;
  double Fraction = 0;
  double Exponent = 0;

  if(ReadBuffer[ReadIndex] == '0'){
    ReadIndex++;
    if(ReadIndex >= ReadSize) return true;
    if(ReadBuffer[ReadIndex] >= '0' && ReadBuffer[ReadIndex] <= '9'){
      ReadError("Invalid number format");
      return false;
    }
  }

  while(ReadIndex < ReadSize){
    if(ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') break;
    Result = 10.0*Result + (ReadBuffer[ReadIndex] - '0');
    ReadIndex++;
  }

  if(ReadBuffer[ReadIndex] == '.'){
    ReadIndex++;
    Fraction = 0.1;
    while(ReadIndex < ReadSize){
      if(ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') break;
      Result += Fraction * (ReadBuffer[ReadIndex] - '0');
      Fraction /= 10.0;
      ReadIndex++;
    }
  }

  if(ReadBuffer[ReadIndex] == 'e' || ReadBuffer[ReadIndex] == 'E'){
    ReadIndex++;
    if(ReadIndex >= ReadSize){
      ReadError("Incomplete number");
      return false;
    }
    if(ReadBuffer[ReadIndex] == '-' || ReadBuffer[ReadIndex] == '+'){
      ExponentSign = true;
      ReadIndex++;
    }
    if(ReadIndex >= ReadSize){
      ReadError("Incomplete number");
      return false;
    }
    while(ReadIndex < ReadSize){
      if(ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') break;
      Exponent = 10.0*Exponent + (ReadBuffer[ReadIndex] - '0');
      ReadIndex++;
    }
  }

  if(ExponentSign)    Exponent *= -1.0;
  if(Exponent != 0.0) Result   *= pow(10.0, Exponent);
  if(Sign)            Result   *= -1.0;

  *Value = Result;
  return true;
}
//------------------------------------------------------------------------------

bool JSON::ReadObject(JSON* ObjectList){
  ReadSpace();
  if(ReadIndex >= ReadSize) return false;

  if(ReadBuffer[ReadIndex] != '{') return false;
  ReadIndex++;

  ObjectList->Clear();
  ObjectList->Type = typeObject;

  string Name;

  ReadSpace();
  if(ReadIndex < ReadSize && ReadBuffer[ReadIndex] == '}'){
    ReadIndex++;
    return true;
  }

  while(ReadIndex < ReadSize){
    if(!ReadString(&Name)){
      ReadError("String expected");
      return false;
    }
    ReadSpace();
    if(ReadBuffer[ReadIndex] != ':'){
      ReadError("\":\" expected");
      return false;
    }
    ReadIndex++;
    if(ReadIndex >= ReadSize){
      ReadError("Incomplete object");
      return false;
    }
    ObjectList->AddOrUpdate(Name.c_str());
    if(!ReadValue((*ObjectList)[Name.c_str()])){
      ReadError("Value expected");
      return false;
    }
    ReadSpace();
    if(ReadIndex < ReadSize && ReadBuffer[ReadIndex] == '}'){
      ReadIndex++;
      return true;
    }
    if(ReadIndex >= ReadSize || ReadBuffer[ReadIndex] != ','){
      ReadError("\",\" expected");
      return false;
    }
    ReadIndex++;
  }
  ReadError("Incomplete object");
  return false;
}
//------------------------------------------------------------------------------

bool JSON::ReadArray(JSON* ObjectList){
  ReadSpace();
  if(ReadIndex >= ReadSize) return false;

  if(ReadBuffer[ReadIndex] != '[') return false;
  ReadIndex++;

  ObjectList->Clear();
  ObjectList->Type = typeArray;

  JSON Value;

  ReadSpace();
  if(ReadIndex < ReadSize && ReadBuffer[ReadIndex] == ']'){
    ReadIndex++;
    return true;
  }

  while(ReadIndex < ReadSize){
    if(!ReadValue(&Value)){
      ReadError("Value expected");
      return false;
    }
    ObjectList->Append(Value);
    ReadSpace();
    if(ReadIndex < ReadSize && ReadBuffer[ReadIndex] == ']'){
      ReadIndex++;
      return true;
    }
    ReadSpace();
    if(ReadIndex >= ReadSize || ReadBuffer[ReadIndex] != ','){
      ReadError("\",\" expected");
      return false;
    }
    ReadIndex++;
  }
  ReadError("Incomplete object");
  return false;
}
//------------------------------------------------------------------------------

bool JSON::ReadValue(JSON* Value){
  ReadSpace();
  if(ReadIndex >= ReadSize) return false;

  if(ReadString(Value)) return true;
  if(ReadNumber(Value)) return true;
  if(ReadObject(Value)) return true;
  if(ReadArray (Value)) return true;

  if(ReadIndex+4 <= ReadSize && !strncmp(ReadBuffer+ReadIndex, "true", 4)){
    ReadIndex += 4;
    Value->Type = typeTrue;
    return true;
  }
  if(ReadIndex+5 <= ReadSize && !strncmp(ReadBuffer+ReadIndex, "false", 5)){
    ReadIndex += 5;
    Value->Type = typeFalse;
    return true;
  }
  if(ReadIndex+4 <= ReadSize && !strncmp(ReadBuffer+ReadIndex, "null", 4)){
    ReadIndex += 4;
    Value->Type = typeNull;
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------

bool JSON::Parse(const char* json){
  Clear();

  ReadSize   = strlen(json);
  ReadBuffer = json;

  ReadLine  = 1;
  ReadIndex = 0;
  if(!ReadValue(this)){
    ReadError("Cannot parse JSON file");
    return false;
  }
  return true;
}
//------------------------------------------------------------------------------

const char* JSON::Stringify(){
  char s[0x100];
  OBJECT* Object;
  ARRAY * Item;

  switch(Type){
    case typeNull:
      Stringification = "null";
      break;

    case typeTrue:
      Stringification = "true";
      break;

    case typeFalse:
      Stringification = "false";
      break;

    case typeString:
      Stringification  = '"';
      for(int n = 0; String[n]; n++){
        switch(String[n]){
          case  '"': Stringification += "\\\""   ; break;
          case '\\': Stringification += "\\\\"   ; break;
          case  '/': Stringification += "\\/"    ; break;
          case '\b': Stringification += "\\b"    ; break;
          case '\f': Stringification += "\\f"    ; break;
          case '\n': Stringification += "\\n"    ; break;
          case '\r': Stringification += "\\r"    ; break;
          case '\t': Stringification += "\\t"    ; break;
          default  : Stringification += String[n]; break;
        }
      }
      Stringification += '"';
      break;

    case typeNumber:
      sprintf(s, "%lg", Number);
      Stringification = s;
      break;

    case typeObject:
      Stringification = "{";
      Object = Objects;
      while(Object){
        Stringification += '"';
        Stringification += Object->Name;
        Stringification += '"';
        Stringification += ":";
        Stringification += Object->Value->Stringify();
        if(Object->Next) Stringification += ",";
        Object = Object->Next;
      }
      Stringification += "}";
      break;

    case typeArray:
      Stringification = "[";
      Item = Items;
      while(Item){
        Stringification += Item->Value->Stringify();
        if(Item->Next) Stringification += ",";
        Item = Item->Next;
      }
      Stringification += "]";
      break;

    default:
      break;
  }
  return Stringification.c_str();
}
//------------------------------------------------------------------------------

