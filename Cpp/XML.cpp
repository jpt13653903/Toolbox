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

#include "XML.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

#define en_dash "\xE2\x80\x93"
#define em_dash "\xE2\x80\x94"
//------------------------------------------------------------------------------

static int EntityCompare(void* left, void* right)
{
    Xml::Entity* leftEntity  = (Xml::Entity*)left;
    Xml::Entity* rightEntity = (Xml::Entity*)right;

    return leftEntity->name.compare(rightEntity->name);
}
//------------------------------------------------------------------------------

static int AttributeCompare(void* left, void* right)
{
    Xml::Attribute* leftAttr  = (Xml::Attribute*)left;
    Xml::Attribute* rightAttr = (Xml::Attribute*)right;

    return leftAttr->name.compare(rightAttr->name);
}
//------------------------------------------------------------------------------

Xml::Entity::Entity(const char* name)
{
    this->name += name;

    children  .compare = EntityCompare;
    attributes.compare = AttributeCompare;
}
//------------------------------------------------------------------------------

Xml::Entity::~Entity()
{
    Entity* child = (Entity*)children.first();
    while(child){
        delete child;
        child = (Entity*)children.next();
    }
    children.clear();

    Attribute* attribute = (Attribute*)attributes.first();
    while(attribute){
        delete attribute;
        attribute = (Attribute*)attributes.next();
    }
    attributes.clear();
}
//------------------------------------------------------------------------------

Xml::Attribute::Attribute(const char* name, const char* value)
{
    this->name  += name;
    this->value += value;
}
//------------------------------------------------------------------------------

Xml::Attribute::~Attribute()
{
}
//------------------------------------------------------------------------------

Xml::Nesting::Nesting(const char* entityName)
{
    next   = 0;
    entity = new Entity(entityName);
}
//------------------------------------------------------------------------------

Xml::Nesting::~Nesting()
{
    // This happens when End() is called.
    // Entity deleted later.
}
//------------------------------------------------------------------------------

Xml::Xml()
{
    nesting = 0;
    root    = 0;
}
//------------------------------------------------------------------------------

Xml::~Xml()
{
    clear();
}
//------------------------------------------------------------------------------

void Xml::clear()
{
    while(nesting) end();
    if   (root   ) delete root;
    root = 0;
}
//------------------------------------------------------------------------------

void Xml::newDocument(const char* document)
{
    clear();

    string legalName;
    getLegalName(document, &legalName);

    nesting = new Nesting(legalName.c_str());
    root    = nesting->entity;
}
//------------------------------------------------------------------------------

void Xml::begin(const char* entity)
{
    if(!nesting) return;

    string legalName;
    getLegalName(entity, &legalName);

    Nesting* temp = new Nesting(legalName.c_str());
    nesting->entity->children.insert(temp->entity);

    temp->next = nesting;
    nesting    = temp;
}
//------------------------------------------------------------------------------

void Xml::comment(const char* comment)
{
    if(!nesting) return;

    nesting->entity->comments += "<!-- ";

    int j;
    for(j = 0; comment[j]; j++){
        if(comment[j] != '-' || comment[j+1] != '-'){
            if(comment[j] == '\n') nesting->entity->comments += "\n     ";
            else                   nesting->entity->comments += comment[j];

        }else{
            if(comment[j+2] == '-'){
                nesting->entity->comments += em_dash;
                j += 2;
            }else{
                nesting->entity->comments += en_dash;
                j++;
            }
        }
    }
    nesting->entity->comments += " -->\n";
}
//------------------------------------------------------------------------------

void Xml::attribute(const char* name, int value)
{
    if(!nesting) return;

    char s[0x100];
    sprintf(s, "%d", value);
    attribute(name, s);
}
//------------------------------------------------------------------------------

void Xml::attribute(const char* name, bool value)
{
    if(!nesting) return;

    if(value) attribute(name, "1");
    else      attribute(name, "0");
}
//------------------------------------------------------------------------------

void Xml::attribute(const char* name, double value)
{
    if(!nesting) return;

    char s[0x100];
    sprintf(s, "%g", value);
    attribute(name, s);
}
//------------------------------------------------------------------------------

void Xml::attribute(const char* name, unsigned value)
{
    if(!nesting) return;

    char s[0x100];
    sprintf(s, "0x%08X", value);
    attribute(name, s);
}
//------------------------------------------------------------------------------

void Xml::attribute(const char* name, const char* value)
{
    if(!nesting) return;

    string legalName;
    getLegalName(name, &legalName);

    Attribute* temp = new Attribute(legalName.c_str(), value);
    nesting->entity->attributes.insert(temp);
}
//------------------------------------------------------------------------------

void Xml::attribute(const char* name, string* value)
{
    attribute(name, value->c_str());
}
//------------------------------------------------------------------------------

void Xml::content(int value)
{
    if(!nesting) return;

    char s[0x100];
    sprintf(s, "%d", value);
    content(s);
}
//------------------------------------------------------------------------------

void Xml::content(bool value)
{
    if(!nesting) return;

    if(value) content("1");
    else      content("0");
}
//------------------------------------------------------------------------------

void Xml::content(double value)
{
    if(!nesting) return;

    char s[0x100];
    sprintf(s, "%g", value);
    content(s);
}
//------------------------------------------------------------------------------

void Xml::content(unsigned value)
{
    if(!nesting) return;

    char s[0x100];
    sprintf(s, "0x%08X", value);
    content(s);
}
//------------------------------------------------------------------------------

void Xml::content(const char* value)
{
    if(!nesting) return;
    if(!value  ) return;

    nesting->entity->content += value;
}
//------------------------------------------------------------------------------

void Xml::end()
{
    if(!nesting) return;

    Nesting* temp = nesting;
    nesting = nesting->next;
    delete temp;
}
//------------------------------------------------------------------------------

void Xml::getLegalName(const char* name, string* legalName)
{
    string temp;
    temp = name;

  *legalName = "";

    int j = 0;

    if(
        (temp[0] == ':'                          ) ||
        (temp[0] == '_'                          ) ||
        (temp[0] >= 'A'     && temp[0] <= 'Z'    ) ||
        (temp[0] >= 'a'     && temp[0] <= 'z'    ) ||
        (temp[0] >= 0x000C0 && temp[0] <= 0x000D6) ||
        (temp[0] >= 0x000D8 && temp[0] <= 0x000F6) ||
        (temp[0] >= 0x000F8 && temp[0] <= 0x002FF) ||
        (temp[0] >= 0x00370 && temp[0] <= 0x0037D) ||
        (temp[0] >= 0x0037F && temp[0] <= 0x01FFF) ||
        (temp[0] >= 0x0200C && temp[0] <= 0x0200D) ||
        (temp[0] >= 0x02070 && temp[0] <= 0x0218F) ||
        (temp[0] >= 0x02C00 && temp[0] <= 0x02FEF) ||
        (temp[0] >= 0x03001 && temp[0] <= 0x0D7FF) ||
        (temp[0] >= 0x0F900 && temp[0] <= 0x0FDCF) ||
        (temp[0] >= 0x0FDF0 && temp[0] <= 0x0FFFD) ||
        (temp[0] >= 0x10000 && temp[0] <= 0xEFFFF)
    )    *legalName += temp[0];
    else *legalName += '_'    ;

    j = 1;
    while(temp[j]){
        if(
            (temp[j] >= '.'                          ) ||
            (temp[j] == ':'                          ) ||
            (temp[j] >= '-'                          ) ||
            (temp[j] == '_'                          ) ||
            (temp[j] >= '0'     && temp[j] <= '9'    ) ||
            (temp[j] >= 'A'     && temp[j] <= 'Z'    ) ||
            (temp[j] >= 'a'     && temp[j] <= 'z'    ) ||
            (temp[j] >= 0x000B7                      ) ||
            (temp[j] >= 0x000C0 && temp[j] <= 0x000D6) ||
            (temp[j] >= 0x000D8 && temp[j] <= 0x000F6) ||
            (temp[j] >= 0x000F8 && temp[j] <= 0x002FF) ||
            (temp[j] >= 0x00300 && temp[j] <= 0x0037D) ||
            (temp[j] >= 0x0037F && temp[j] <= 0x01FFF) ||
            (temp[j] >= 0x0200C && temp[j] <= 0x0200D) ||
            (temp[j] >= 0x0203F && temp[j] <= 0x02040) ||
            (temp[j] >= 0x02070 && temp[j] <= 0x0218F) ||
            (temp[j] >= 0x02C00 && temp[j] <= 0x02FEF) ||
            (temp[j] >= 0x03001 && temp[j] <= 0x0D7FF) ||
            (temp[j] >= 0x0F900 && temp[j] <= 0x0FDCF) ||
            (temp[j] >= 0x0FDF0 && temp[j] <= 0x0FFFD) ||
            (temp[j] >= 0x10000 && temp[j] <= 0xEFFFF)
        )    *legalName += temp[j];
        else *legalName += '_'    ;
        j++;
    }
}
//------------------------------------------------------------------------------

void Xml::getLegalContent(
    string* content,
    string* legalContent
){
    *legalContent = "";

    int         j;
    const char* temp = content->c_str();
    for(j = 0; temp[j]; j++){
        switch(temp[j]){
            case '<':
                *legalContent += "&lt";
                break;

            case '>':
                *legalContent += "&gt";
                break;

            case '"':
                *legalContent += "&quot";
                break;

            case '\'':
                *legalContent += "&apos";
                break;

            case '&':
                *legalContent += "&amp";
                break;

            default:
                *legalContent += temp[j];
                break;
        }
    }
}
//------------------------------------------------------------------------------

void Xml::saveEntity(Entity* entity, unsigned indent)
{
    unsigned j;

    if(!entity->comments.empty()){
        const char* s  = entity->comments.c_str();
        unsigned    i1 = 0;
        unsigned    i2 = 0;

        // Comments are "\n\0" terminated
        while(s[i2]){
            if(s[i2] == '\n'){
                for(j = 0; j < indent; j++) buffer += "  ";
                while(i1 < i2) buffer += s[i1++];
                i1      = ++i2;
                buffer += '\n';

            }else{
                i2++;
            }
        }
    }

    for(j = 0; j < indent; j++) buffer += "  ";
    buffer += '<';
    buffer += entity->name;

    // Add Attributes
    if(entity->attributes.itemCount()){
        unsigned   equalPos = 0;
        Attribute* temp     = (Attribute*)entity->attributes.first();
        while(temp){
            getLegalContent(&temp->value, &temp->legalValue);
            j = temp->name.length();
            if(equalPos < j) equalPos = j;
            temp = (Attribute*)entity->attributes.next();
        }

        temp = (Attribute*)entity->attributes.first();
        while(temp){
            buffer += "\n  ";
            for(j = 0; j < indent; j++) buffer += "  ";
            buffer += temp->name;

            j = temp->name.length();
            for(; j < equalPos ; j++) buffer += ' ';
            buffer += " = \"";

            buffer += temp->legalValue;
            buffer += '"';
            temp = (Attribute*)entity->attributes.next();
        }
        buffer += '\n';
        for(j = 0; j < indent; j++) buffer += "  ";
    }

    // Add children and then the content
    if(!entity->content.empty() || entity->children.itemCount()){
        buffer += ">\n";

        if(!entity->content.empty()){
            getLegalContent(&entity->content, &entity->legalContent);

            const char* s  = entity->legalContent.c_str();
            unsigned    i1 = 0;
            unsigned    i2 = 0;

            while(true){
                if(s[i2] == '\n'){
                    for(j = 0; j <= indent; j++) buffer += "  ";
                    while(i1 < i2) buffer += s[i1++];
                    i1      = ++i2;
                    buffer += '\n';

                }else if(s[i2] == 0){
                    for(j = 0; j <= indent; j++) buffer += "  ";
                    if(s[i1]) buffer += s + i1;
                    buffer += '\n';
                    break;

                }else{
                    i2++;
                }
            }
        }

        if(entity->children.itemCount()){
            Entity* temp = (Entity*)entity->children.first();
            while(temp){
                saveEntity(temp, indent + 1);
                temp = (Entity*)entity->children.next();
            }
        }
        for(j = 0; j < indent; j++) buffer += "  ";
        buffer += "</";
        buffer += entity->name;
        buffer += ">\n";

    }else{
        buffer += "/>\n";
    }
}
//------------------------------------------------------------------------------

bool Xml::save(const char* filename)
{
    if(!root) return false;

    while(nesting) end();

    FILE* file = fopen(filename, "wb");
    if(!file) return false;

    buffer += "<?"
              "xml version ""= \"1.0\" "
              "encoding "   "= \"UTF-8\" "
              "standalone " "= \"yes\" "
              "?>\n";
    saveEntity(root);
    fwrite(buffer.c_str(), 1, buffer.length(), file);
    fclose(file);

    // Clear memory
    buffer = "";

    return true;
}
//------------------------------------------------------------------------------

void Xml::printError(const char* message)
{
    unsigned n = 0;
    int   line = 1;
    for(n = 0; n < readIndex; n++){
        if(readBuffer[n] == '\n') line++;
    }
    error("XML Error\n  %s\n  Line: %d", message, line);
}
//------------------------------------------------------------------------------

bool Xml::readSpace()
{
    if(readIndex >= readSize) return false;

    if(
        readBuffer[readIndex] == ' '  ||
        readBuffer[readIndex] == '\t' ||
        readBuffer[readIndex] == '\r' ||
        readBuffer[readIndex] == '\n'
    ){
        readIndex++;
        return true;
    }
    if( // Check for zero-width no-break space
        readBuffer[readIndex  ] == '\xEF' &&
        readBuffer[readIndex+1] == '\xBB' &&
        readBuffer[readIndex+2] == '\xBF'
    ){
        readIndex += 3;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Xml::readComment()
{
    if(readIndex >= readSize) return false;

    if(
        readBuffer[readIndex  ] == '<' &&
        readBuffer[readIndex+1] == '!' &&
        readBuffer[readIndex+2] == '-' &&
        readBuffer[readIndex+3] == '-'
    ){
        readIndex += 4;

        while(readIndex < readSize){
            if(
                readBuffer[readIndex  ] == '-' &&
                readBuffer[readIndex+1] == '-' &&
                readBuffer[readIndex+2] == '>'
            ){
                readIndex += 3;
                return true;
            }
            readIndex++;
        }
    }

    return false;
}
//------------------------------------------------------------------------------

bool Xml::readSpecial()
{
    if(readIndex >= readSize) return false;

    int nestLevel = 0;

    if(
        readBuffer[readIndex  ] == '<' &&
        readBuffer[readIndex+1] == '!' &&
        readBuffer[readIndex+2] != '-'
    ){
        readIndex += 3;
        nestLevel ++;

        while(readIndex < readSize){
            switch(readBuffer[readIndex]){
                case '<':
                    nestLevel++;
                    break;

                case '>':
                    nestLevel--;
                    if(!nestLevel){
                        readIndex++;
                        return true;
                    }
                    break;

                default:
                    break;

            }
            readIndex++;
        }
    }

    return false;
}
//------------------------------------------------------------------------------

bool Xml::readName(string* buffer)
{
  *buffer = "";

    while(readSpace() || readComment());

    while(readIndex < readSize){
        if(
            readBuffer[readIndex] <= ' ' ||
            readBuffer[readIndex] == '=' ||
            readBuffer[readIndex] == '<' ||
            readBuffer[readIndex] == '>' ||
            readBuffer[readIndex] == '?' ||
            readBuffer[readIndex] == '!' ||
            readBuffer[readIndex] == '/'
        ){
            break;
        }
        *buffer += readBuffer[readIndex++];
    }

    return buffer->length();
}
//------------------------------------------------------------------------------

bool Xml::readContent(string* buffer, char end)
{
    while(readIndex < readSize){
        if(
            readBuffer[readIndex] == '<' ||
            readBuffer[readIndex] == '>' ||
            readBuffer[readIndex] == end
        ){
        break;

        }else if(readBuffer[readIndex] == '&'){
            readIndex++;
            if(
                readBuffer[readIndex  ] == 'q' &&
                readBuffer[readIndex+1] == 'u' &&
                readBuffer[readIndex+2] == 'o' &&
                readBuffer[readIndex+3] == 't'
            ){
                readIndex += 4;
                *buffer += '"';

            }else if(
                readBuffer[readIndex  ] == 'a' &&
                readBuffer[readIndex+1] == 'p' &&
                readBuffer[readIndex+2] == 'o' &&
                readBuffer[readIndex+3] == 's'
            ){
                readIndex += 4;
                *buffer += '\'';

            }else if(
                readBuffer[readIndex  ] == 'l' &&
                readBuffer[readIndex+1] == 't'
            ){
                readIndex += 2;
                *buffer += '<';

            }else if(
                readBuffer[readIndex  ] == 'g' &&
                readBuffer[readIndex+1] == 't'
            ){
                readIndex += 2;
                *buffer += '>';

            }else if(
                readBuffer[readIndex  ] == 'a' &&
                readBuffer[readIndex+1] == 'm' &&
                readBuffer[readIndex+2] == 'p'
            ){
                readIndex += 3;
                *buffer += '&';
            }

        }else{
            *buffer += readBuffer[readIndex++];
        }
    }

    return buffer->length();
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(LLRBTree* tree)
{
    Attribute* temp = new Attribute("", "");

    if(!readName(&temp->name)){ // No name
        delete temp;
        return false;
    }

    while(readSpace() || readComment());

    if(readBuffer[readIndex] != '='){ // No assignment
        delete temp;
        printError("No Assignment");
        return false;
    }
    readIndex++;

    while(readSpace() || readComment());

    if( // No value
        readBuffer[readIndex] != '"' &&
        readBuffer[readIndex] != '\''
    ){
        delete temp;
        printError("No Value");
        return false;
    }

    char end = readBuffer[readIndex++];

    readContent(&temp->value, end);

    if(readBuffer[readIndex] != end){ // Open string
        delete temp;
        printError("Open String");
        return false;
    }
    readIndex++;

    if(tree->find(temp)){ // Duplicate entry
        delete temp;
        printError("Duplicate Attribute");
        return false;
    }

    tree->insert(temp);
    return true;
}
//------------------------------------------------------------------------------

bool Xml::readHeader()
{
    while(readSpace() || readComment());

    if(
        readBuffer[readIndex  ] == '<' &&
        readBuffer[readIndex+1] == '?' &&
        readBuffer[readIndex+2] == 'x' &&
        readBuffer[readIndex+3] == 'm' &&
        readBuffer[readIndex+4] == 'l'
    ){
        readIndex += 5;

        LLRBTree attributes;
        attributes.compare = AttributeCompare;

        while(readAttribute(&attributes));

        while(readSpace() || readComment());

        if(
            readBuffer[readIndex  ] == '?' &&
            readBuffer[readIndex+1] == '>'
        ){
            readIndex += 2;

            Attribute  key("version", "");
            Attribute* attribute = (Attribute*)attributes.find(&key);
            if(!attribute){
                printError("No version");
                return false;
            }

            key.name  = "encoding";
            attribute = (Attribute*)attributes.find(&key);
            if(!attribute){
                warning("No encoding, assuming UTF-8");
                return true;
            }
            if(attribute->value != "UTF-8"){
                printError("Encoding other than UTF-8");
                return false;
            }
            return true;
        }
    }
    warning("No header, assuming defaults");
    return true;
}
//------------------------------------------------------------------------------

Xml::Entity* Xml::readEntity()
{
    while(readSpace() || readComment() || readSpecial());

    if(
        readBuffer[readIndex  ] != '<' || // Not an entity tag
        readBuffer[readIndex+1] == '/'    // Closing tag
    ) return 0;
    readIndex++;

    string buffer;
    if(!readName(&buffer)){
        printError("Invalid tag");
        return 0;
    }

    Entity* entity = new Entity(buffer.c_str());
    while(readAttribute(&entity->attributes));

    while(readSpace() || readComment());

    if(
        readBuffer[readIndex  ] == '/' &&
        readBuffer[readIndex+1] == '>'
    ){
        readIndex += 2;
        return entity;
    }

    if(readBuffer[readIndex] != '>'){
        printError("Invalid tag");
        delete entity;
        return 0;
    }
    readIndex++;

    bool foundBody;
    while(readIndex < readSize){
        foundBody = false;

        while(readSpace() || readComment());
        if(
            readBuffer[readIndex  ] == '<' &&
            readBuffer[readIndex+1] == '/'
        ){
            readIndex += 2;
            buffer = "";
            if(!readName(&buffer)){
                printError("Invalid closing tag");
                delete entity;
                return 0;
            }
            if(readBuffer[readIndex] != '>'){
                printError("Invalid closing tag");
                delete entity;
                return 0;
            }
            readIndex++;
            if(buffer != entity->name){
                printError("Closing tag does not match opening tag");
                delete entity;
                return 0;
            }
            return entity;
        }

        Entity* child = readEntity();
        if(child){
            entity->children.insert(child);
            foundBody = true;
        }

        buffer = "";
        if(readContent(&buffer)){
            entity->content += buffer;
            foundBody = true;
        }

        if(!foundBody) break;
    }

    printError("No closing tag");
    delete entity;
    return 0;
}
//------------------------------------------------------------------------------

bool Xml::load(const char* filename)
{
    clear();

    FILE* file = fopen(filename, "rb");
    if(!file) return false;

    fseek(file, 0, SEEK_END);
    readSize   = ftell(file);
    fseek(file, 0, SEEK_SET);
    readBuffer = new char[readSize];

    if(!readBuffer){
        fclose(file);
        return false;
    }

    if(!fread(readBuffer, 1, readSize, file)){
        fclose(file);
        delete[] readBuffer;
        return   false;
    }
    fclose(file);

    readIndex = 0;
    if(!readHeader()){
        printError("No header");
        delete[] readBuffer;
        return   false;
    }

    root = readEntity();
    if(!root){
        printError("No root entity");
        delete[] readBuffer;
        return   false;
    }

    // Clear memory
    delete[] readBuffer;
    return   true;
}
//------------------------------------------------------------------------------

Xml::Entity* Xml::findChild(Entity* entity, const char* name)
{
    if(!entity) return 0;

    string legalName;
    getLegalName(name, &legalName);

    Entity key(legalName.c_str());
    return (Entity*)entity->children.find(&key);
}
//------------------------------------------------------------------------------

Xml::Entity* Xml::nextChild(Entity* entity, const char* name)
{
    if(!entity) return 0;

    string legalName;
    getLegalName(name, &legalName);

    Entity* result = (Entity*)entity->children.next();
    if(result && result->name == legalName) return result;
    return 0;
}
//------------------------------------------------------------------------------

Xml::Attribute* Xml::findAttribute(
    Entity* entity, const char* name
){
    if(!entity) return 0;

    string legalName;
    getLegalName(name, &legalName);

    Attribute key(legalName.c_str(), "");
    return (Attribute*)entity->attributes.find(&key);
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(
    Entity*     entity,
    const char* name,
    int*        value
){
    Attribute* a = findAttribute(entity, name);
    if(a){
        *value = calc.calculate(a->value.c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(
    Entity*     entity,
    const char* name,
    bool*       value
){
    int temp;
    if(readAttribute(entity, name, &temp)){
        *value = temp;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(
    Entity*     entity,
    const char* name,
    char*       value
){
    Attribute* a = findAttribute(entity, name);
    if(a){
        strcpy(value, a->value.c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(
    Entity*     entity,
    const char* name,
    string*     value
){
    Attribute* a = findAttribute(entity, name);
    if(a){
        *value = a->value;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(
    Entity*     entity,
    const char* name,
    double*     value
){
    Attribute* a = findAttribute(entity, name);
    if(a){
        *value = calc.calculate(a->value.c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

bool Xml::readAttribute(
    Entity*     entity,
    const char* name,
    unsigned*   value
){
    Attribute* a = findAttribute(entity, name);
    if(a){
        *value = calc.calculate(a->value.c_str());
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------

