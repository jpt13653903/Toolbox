/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#include "JSON.h"
//------------------------------------------------------------------------------

bool operator== (std::string& a, const char* b){ return !(a.compare(b)); }
//------------------------------------------------------------------------------

// Converts UTF-32 to UTF-8 and appends it to the string
void operator+= (std::string& s, uint32_t character)
{
    int  j = 0;
    byte head = 0x3F; // Active bits in the leading byte
    byte lead = 0x80; // Leading byte
    byte cont[6];     // Continuation bytes

    if(character < 0x80){
        s += (char)character;

    }else{
        while(character > head){ // Doesn't fit in the leading byte
            cont[j]     = 0x80 | (character & 0x3F);
            lead        = 0x80 | (lead >> 1);
            head      >>= 1;
            character >>= 6;
            j++;
        }
        s += (char)(lead | character);
        for(j--; j >= 0; j--) s += (char)(cont[j]);
    }
}
void operator+= (std::string& s, uint16_t character){ s += (uint32_t)character; }
//------------------------------------------------------------------------------

Json::Json()
{
    type   = typeNull;
    string = "";
    number = 0;
}
//------------------------------------------------------------------------------

Json::Json(Json& value)
{
    type    = typeNull;
    string  = "";
    number  = 0;
    operator=(value);
}
//------------------------------------------------------------------------------

Json::Json(const char* value)
{
    type    = typeNull;
    string  = "";
    number  = 0;
    operator=(value);
}
//------------------------------------------------------------------------------

Json::Json(int value)
{
    type    = typeNull;
    string  = "";
    number  = 0;
    operator=(value);
}
//------------------------------------------------------------------------------

Json::Json(unsigned value)
{
    type    = typeNull;
    string  = "";
    number  = 0;
    operator=(value);
}
//------------------------------------------------------------------------------

Json::Json(double value)
{
    type    = typeNull;
    string  = "";
    number  = 0;
    operator=(value);
}
//------------------------------------------------------------------------------

Json::Json(bool value)
{
    type    = typeNull;
    string  = "";
    number  = 0;
    operator=(value);
}
//------------------------------------------------------------------------------

Json::~Json()
{
    clear();
}
//------------------------------------------------------------------------------

void Json::clear()
{
    string = "";
    number = 0;

    for(
        auto object = objects.begin();
        object != objects.end();
        object++
    ) delete object->second;
    objects.clear();

    for(size_t n = 0; n < items.size(); n++) delete items[n];
    items.clear();

    type = typeNull;
}
//------------------------------------------------------------------------------

void Json::operator=(Json& value)
{
    clear();
    type = value.type;

    switch(value.type){
        case typeNull:
        case typeTrue:
        case typeFalse:
            break;

        case typeString:
            string = value.string;
            break;

        case typeNumber:
            number = value.number;
            break;

        case typeObject:
            for(
                auto object = value.objects.begin();
                object != value.objects.end();
                object++
            ) addOrUpdate(object->first.c_str(), *(object->second));
            break;

        case typeArray:
            for(size_t n = 0; n < value.items.size(); n++) append(*(value.items[n]));
            break;

        default:
            break;
    }
}
//------------------------------------------------------------------------------

void Json::operator=(const char* value)
{
    clear();
    type   = typeString;
    string = value;
}
//------------------------------------------------------------------------------

void Json::operator=(int value)
{
    clear();
    type   = typeNumber;
    number = value;
}
//------------------------------------------------------------------------------

void Json::operator=(unsigned value)
{
    clear();
    type   = typeNumber;
    number = value;
}
//------------------------------------------------------------------------------

void Json::operator=(double value)
{
    clear();
    type   = typeNumber;
    number = value;
}
//------------------------------------------------------------------------------

void Json::operator=(bool value)
{
    clear();
    if(value) type = typeTrue;
    else      type = typeFalse;
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(Json& value)
{
    if(value.type == typeObject){
        for(auto object: value.objects)
            addOrUpdate(object.first.c_str(), *(object.second));
    }else{
        operator=(value);
    }
    return this;
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name, Json& value)
{
    type = typeObject;

    Json* json = operator[](name);
    if(json) return json->addOrUpdate(value);

    Json* object  = new Json(value);
    objects[name] = object;

    return object;
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name, const char* value)
{
    Json json(value); return addOrUpdate(name, json);
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name, int value)
{
    Json json(value); return addOrUpdate(name, json);
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name, unsigned value)
{
    Json json(value); return addOrUpdate(name, json);
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name, double value)
{
    Json json(value); return addOrUpdate(name, json);
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name, bool value)
{
    Json json(value); return addOrUpdate(name, json);
}
//------------------------------------------------------------------------------

Json* Json::addOrUpdate(const char* name)
{
    Json* json = operator[](name);
    if(json)   return json;
    Json null; return addOrUpdate(name, null);
}
//------------------------------------------------------------------------------

Json* Json::operator[] (const char* name)
{
    auto object = objects.find(name);
    if(object == objects.end()) return 0;
    return object->second;
}
//------------------------------------------------------------------------------

void Json::append(Json& value)
{
    // if the array is empty, add any existing object as the first item in the array
    if(items.empty() && type != typeNull && type != typeArray){
        Json* item = new Json(*this);
        items.push_back(item);
    }
    type = typeArray;

    Json* item = new Json(value);
    items.push_back(item);
}
//------------------------------------------------------------------------------

void Json::append(const char* value)
{
    Json json(value); append(json);
}
//------------------------------------------------------------------------------

void Json::append(int value)
{
    Json json(value); append(json);
}
//------------------------------------------------------------------------------

void Json::append(unsigned value)
{
    Json json(value); append(json);
}
//------------------------------------------------------------------------------

void Json::append(double value)
{
    Json json(value); append(json);
}
//------------------------------------------------------------------------------

void Json::append(bool value)
{
    Json json(value); append(json);
}
//------------------------------------------------------------------------------

Json* Json::operator[] (int index)
{
    if(index < 0 || (size_t)index >= items.size()) return 0;
    return items[index];
}
//------------------------------------------------------------------------------

void Json::readError(const char* message)
{
    static bool firstError = true;
    if(firstError) error("JSON error on line %u:\n  %s", readLine, message);
    firstError = false;
}
//------------------------------------------------------------------------------

void Json::readLineComment()
{
    readIndex += 2;
    while(readIndex < readSize){
        switch(readBuffer[readIndex]){
            case '\n':
                readLine++;
                readIndex++;
                if(readBuffer[readIndex] == '\r') readIndex++;
                return;

            case '\r':
                readLine++;
                readIndex++;
                if(readBuffer[readIndex] == '\n') readIndex++;
                return;

            default:
                readIndex++;
                break;
        }
    }
}
//------------------------------------------------------------------------------

void Json::readBlockComment()
{
    readIndex += 2;
    while(readIndex < readSize){
        switch(readBuffer[readIndex]){
            case '\n':
                readLine++;
                readIndex++;
                if(readBuffer[readIndex] == '\r') readIndex++;
                break;

            case '\r':
                readLine++;
                readIndex++;
                if(readBuffer[readIndex] == '\n') readIndex++;
                break;

            case '*':
                readIndex++;
                if(readBuffer[readIndex] == '/'){
                    readIndex++;
                    return;
                }
                break;

            default:
                readIndex++;
                break;
        }
    }
}
//------------------------------------------------------------------------------

void Json::readSpace()
{
    while(readIndex < readSize){
        switch(readBuffer[readIndex]){
            case '\n': readLine++;
            case '\r':
            case '\t':
            case '\v':
            case ' ' :
                readIndex++;
                break;

            case '/':
                switch(readBuffer[readIndex+1]){
                    case '/':
                        readLineComment();
                        break;
                    case '*':
                        readBlockComment();
                        break;
                    default:
                        return;
                }
                break;

            default:
                return;
        }
    }
}
//------------------------------------------------------------------------------

bool Json::readUnicodeSequence(std::string* string)
{
    uint16_t character = 0;

    if(readBuffer[readIndex] != 'u') return false;

    for(int n = 0; n < 4; n++){
        readIndex++;
        if(readIndex >= readSize) return false;

        if      (readBuffer[readIndex] >= '0' && readBuffer[readIndex] <= '9'){
            character = 0x10*character + (readBuffer[readIndex] - '0');
        }else if(readBuffer[readIndex] >= 'a' && readBuffer[readIndex] <= 'f'){
            character = 0x10*character + (readBuffer[readIndex] - 'a' + 0xA);
        }else if(readBuffer[readIndex] >= 'A' && readBuffer[readIndex] <= 'F'){
            character = 0x10*character + (readBuffer[readIndex] - 'A' + 0xA);
        }else{
            return false;
        }
    }
    *string += character;
    return true;
}
//------------------------------------------------------------------------------

bool Json::readIdentifierStart(std::string* string)
{
    if('A' <= readBuffer[readIndex] && readBuffer[readIndex] <= 'Z'){
        *string += readBuffer[readIndex++];
        return true;
    }
    if('a' <= readBuffer[readIndex] && readBuffer[readIndex] <= 'z'){
        *string += readBuffer[readIndex++];
        return true;
    }
    if(readBuffer[readIndex] == '$' || readBuffer[readIndex] == '_'){
        *string += readBuffer[readIndex++];
        return true;
    }
    // this is actually wrong, but works when the input is assumed valid
    if(readBuffer[readIndex] & 0xC0){
        *string += readBuffer[readIndex++];
        while((readBuffer[readIndex] & 0xC0) == 0x80) *string += readBuffer[readIndex++];
        return true;
    }
    if(readBuffer[readIndex] == '\\'){
        readIndex++;
        if(!readUnicodeSequence(string)){
            readError("Incomplete Unicode escape sequence");
            return false;
        }
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Json::readIdentifierPart(std::string* string)
{
    if(readIdentifierStart(string)) return true;

    if('0' <= readBuffer[readIndex] && readBuffer[readIndex] <= '9'){
        *string += readBuffer[readIndex++];
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Json::readIdentifier(std::string* string)
{
    readSpace();
    if(readIndex >= readSize) return false;

    *string = "";

    if(!readIdentifierStart(string)) return false;

    while(readIdentifierPart(string));

    return true;
}
//------------------------------------------------------------------------------

bool Json::readString(std::string* value)
{
    readSpace();
    if(readIndex >= readSize) return false;

    bool doubleQuotes;
    if     (readBuffer[readIndex] == '"' ) doubleQuotes = true;
    else if(readBuffer[readIndex] == '\'') doubleQuotes = false;
    else return false;
    readIndex++;

    *value = "";

    while(readIndex < readSize){
        switch(readBuffer[readIndex]){
            case '"':
                if(doubleQuotes){
                    readIndex++;
                    return true;
                }
                *value += readBuffer[readIndex];
                break;

            case '\'':
                if(!doubleQuotes){
                    readIndex++;
                    return true;
                }
                *value += readBuffer[readIndex];
                break;

            case '\\':
                readIndex++;
                if(readIndex >= readSize){
                    readError("Incomplete escape sequence");
                    return false;
                }
                switch(readBuffer[readIndex]){
                    case '"' : *value += '"' ; break;
                    case '\'': *value += '\''; break;
                    case '\\': *value += '\\'; break;
                    case '/' : *value += '/' ; break;
                    case 'b' : *value += '\b'; break;
                    case 'f' : *value += '\f'; break;
                    case 'n' : *value += '\n'; break;
                    case 'r' : *value += '\r'; break;
                    case 't' : *value += '\t'; break;
                    case 'u' :
                        if(!readUnicodeSequence(value)){
                            readError("Incomplete Unicode escape sequence");
                            return false;
                        }
                        break;
                    case '\n':
                        readLine++;
                        if(readBuffer[readIndex+1] == '\r') readIndex++;
                        break;
                    case '\r':
                        readLine++;
                        if(readBuffer[readIndex+1] == '\n') readIndex++;
                        break;

                    default:
                        readError("Unrecognised escape sequence");
                        return false;
                }
                break;

            case '\b':
            case '\f':
            case '\n':
            case '\r':
                readError("Unexpected control character in string");
                return false;

            default:
                *value += readBuffer[readIndex];
                break;
        }
        readIndex++;
    }
    readError("Incomplete string");
    return false;
}
//------------------------------------------------------------------------------

bool Json::readString(Json* value)
{
    if(readString(&(value->string))){
        value->type = typeString;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Json::readHexadecimal(Json* value, bool sign)
{
    double result = 0;

    while(readIndex < readSize){
        if      ('0' <= readBuffer[readIndex] && readBuffer[readIndex] <= '9'){
            result *= 16.0;
            result += readBuffer[readIndex++] - '0';
        }else if('a' <= readBuffer[readIndex] && readBuffer[readIndex] <= 'f'){
            result *= 16.0;
            result += readBuffer[readIndex++] - 'a' + 10;
        }else if('A' <= readBuffer[readIndex] && readBuffer[readIndex] <= 'F'){
            result *= 16.0;
            result += readBuffer[readIndex++] - 'A' + 10;
        }else{
            break;
        }
    }
    value->number = sign ? -result : result;
    return true;
}
//------------------------------------------------------------------------------

bool Json::readNumber(Json* value)
{
    readSpace();
    if(readIndex >= readSize) return false;

    if(readIndex+8 <= readSize && !strncmp(readBuffer+readIndex, "Infinity", 8)){
        readIndex += 8;
        value->type   = typeNumber;
        value->number = 1.0/0.0;
        return true;
    }
    if(readIndex+9 <= readSize && !strncmp(readBuffer+readIndex, "-Infinity", 9)){
        readIndex += 9;
        value->type   = typeNumber;
        value->number = -1.0/0.0;
        return true;
    }
    if(readIndex+3 <= readSize && !strncmp(readBuffer+readIndex, "NaN", 3)){
        readIndex += 3;
        value->type   = typeNumber;
        value->number = 0.0/0.0;
        return true;
    }

    bool sign         = false;
    bool exponentSign = false;

    if(readBuffer[readIndex] == '-'){
        sign = true;
        readIndex++;
        if(readIndex >= readSize) return false;
    }else if(readBuffer[readIndex] == '+'){
        sign = false;
        readIndex++;
        if(readIndex >= readSize) return false;
    }

    if((readBuffer[readIndex] <  '0' || readBuffer[readIndex] > '9') &&
            readBuffer[readIndex] != '.' ){
        if(sign) readError("Incomplete number");
        return false;
    }

    if(readIndex+2 <= readSize && !strncmp(readBuffer+readIndex, "0x", 2)){
        readIndex += 2;
        value->type = typeNumber;
        return readHexadecimal(value, sign);
    }
    if(readIndex+2 <= readSize && !strncmp(readBuffer+readIndex, "0X", 2)){
        readIndex += 2;
        value->type = typeNumber;
        return readHexadecimal(value, sign);
    }

    *value = 0;

    double result   = 0;
    double fraction = 0;
    double exponent = 0;

    if(readBuffer[readIndex] == '0'){
        readIndex++;
        if(readIndex >= readSize) return true;
        if(readBuffer[readIndex] >= '0' && readBuffer[readIndex] <= '9'){
            readError("Invalid number format");
            return false;
        }
    }

    while(readIndex < readSize){
        if(readBuffer[readIndex] < '0' || readBuffer[readIndex] > '9') break;
        result = 10.0*result + (readBuffer[readIndex] - '0');
        readIndex++;
    }

    if(readBuffer[readIndex] == '.'){
        readIndex++;
        fraction = 0.1;
        while(readIndex < readSize){
            if(readBuffer[readIndex] < '0' || readBuffer[readIndex] > '9') break;
            result += fraction * (readBuffer[readIndex] - '0');
            fraction /= 10.0;
            readIndex++;
        }
    }

    if(readBuffer[readIndex] == 'e' || readBuffer[readIndex] == 'e'){
        readIndex++;
        if(readIndex >= readSize){
            readError("Incomplete number");
            return false;
        }
        if(readBuffer[readIndex] == '-' || readBuffer[readIndex] == '+'){
            exponentSign = true;
            readIndex++;
        }
        if(readIndex >= readSize){
            readError("Incomplete number");
            return false;
        }
        while(readIndex < readSize){
            if(readBuffer[readIndex] < '0' || readBuffer[readIndex] > '9') break;
            exponent = 10.0*exponent + (readBuffer[readIndex] - '0');
            readIndex++;
        }
    }

    if(exponentSign)    exponent *= -1.0;
    if(exponent != 0.0) result   *= pow(10.0, exponent);
    if(sign)            result   *= -1.0;

    *value = result;
    return true;
}
//------------------------------------------------------------------------------

bool Json::readObject(Json* objectList)
{
    readSpace();
    if(readIndex >= readSize) return false;

    if(readBuffer[readIndex] != '{') return false;
    readIndex++;

    objectList->clear();
    objectList->type = typeObject;

    std::string name;

    readSpace();
    if(readIndex < readSize && readBuffer[readIndex] == '}'){
        readIndex++;
        return true;
    }

    while(readIndex < readSize){
        if(!readString(&name)){
            if(!readIdentifier(&name)){
                readError("String or Identifier expected");
                return false;
            }
        }
        readSpace();
        if(readBuffer[readIndex] != ':'){
            readError("\":\" expected");
            return false;
        }
        readIndex++;
        if(readIndex >= readSize){
            readError("Incomplete object");
            return false;
        }
        if(!readValue(objectList->addOrUpdate(name.c_str()))){
            readError("Value expected");
            return false;
        }
        readSpace();
        if(readIndex < readSize && readBuffer[readIndex] == '}'){
            readIndex++;
            return true;
        }
        if(readIndex >= readSize || readBuffer[readIndex] != ','){
            readError("\",\" expected");
            return false;
        }
        readIndex++;
        readSpace();
        if(readIndex < readSize && readBuffer[readIndex] == '}'){
            readIndex++;
            return true;
        }
    }
    readError("Incomplete object");
    return false;
}
//------------------------------------------------------------------------------

bool Json::readArray(Json* objectList)
{
    readSpace();
    if(readIndex >= readSize) return false;

    if(readBuffer[readIndex] != '[') return false;
    readIndex++;

    objectList->clear();
    objectList->type = typeArray;

    Json value;

    readSpace();
    if(readIndex < readSize && readBuffer[readIndex] == ']'){
        readIndex++;
        return true;
    }

    while(readIndex < readSize){
        if(!readValue(&value)){
            readError("Value expected");
            return false;
        }
        objectList->append(value);
        readSpace();
        if(readIndex < readSize && readBuffer[readIndex] == ']'){
            readIndex++;
            return true;
        }
        readSpace();
        if(readIndex >= readSize || readBuffer[readIndex] != ','){
            readError("\",\" expected");
            return false;
        }
        readIndex++;
        readSpace();
        if(readIndex < readSize && readBuffer[readIndex] == ']'){
            readIndex++;
            return true;
        }
    }
    readError("Incomplete object");
    return false;
}
//------------------------------------------------------------------------------

bool Json::readValue(Json* value)
{
    readSpace();
    if(readIndex >= readSize) return false;

    if(readString(value)) return true;
    if(readNumber(value)) return true;
    if(readObject(value)) return true;
    if(readArray (value)) return true;

    if(readIndex+4 <= readSize && !strncmp(readBuffer+readIndex, "true", 4)){
        readIndex += 4;
        value->type = typeTrue;
        return true;
    }
    if(readIndex+5 <= readSize && !strncmp(readBuffer+readIndex, "false", 5)){
        readIndex += 5;
        value->type = typeFalse;
        return true;
    }
    if(readIndex+4 <= readSize && !strncmp(readBuffer+readIndex, "null", 4)){
        readIndex += 4;
        value->type = typeNull;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Json::parse(const char* json, unsigned length)
{
    clear();

    if(length) readSize = length;
    else       readSize = strlen(json);

    readLine   = 1;
    readIndex  = 0;
    readBuffer = json;

    if(!readValue(this)){
        readError("Cannot parse JSON file");
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

const char* Json::stringify()
{
    int  n, N;
    char s[0x100];

    switch(type){
        case typeNull:
            stringification = "null";
            break;

        case typeTrue:
            stringification = "true";
            break;

        case typeFalse:
            stringification = "false";
            break;

        case typeString:
            stringification  = '"';
            for(n = 0; string[n]; n++){
                switch(string[n]){
                    case  '"': stringification += "\\\""   ; break;
                    case '\\': stringification += "\\\\"   ; break;
                    case  '/': stringification += "\\/"    ; break;
                    case '\b': stringification += "\\b"    ; break;
                    case '\f': stringification += "\\f"    ; break;
                    case '\n': stringification += "\\n"    ; break;
                    case '\r': stringification += "\\r"    ; break;
                    case '\t': stringification += "\\t"    ; break;
                    default  : stringification += string[n]; break;
                }
            }
            stringification += '"';
            break;

        case typeNumber:
            sprintf(s, "%.14g", number);
            stringification = s;
            break;

        case typeObject:
            stringification = '{';
            n = 0;
            N = objects.size();
            for(
                auto object = objects.begin();
                object != objects.end();
                object++
            ){
                stringification += '"';
                stringification += object->first;
                stringification += '"';
                stringification += ':';
                stringification += object->second->stringify();
                n++;
                if(n < N) stringification += ",";
            }
            stringification += '}';
            break;

        case typeArray:
            stringification = "[";
            for(size_t n = 0; n < items.size(); n++){
                stringification += items[n]->stringify();
                if(n < items.size()-1) stringification += ",";
            }
            stringification += "]";
            break;

        default:
            break;
    }
    return stringification.c_str();
}
//------------------------------------------------------------------------------

