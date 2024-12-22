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

// An abstraction of a subset of the XML-1.0 standard.

// The only supported encoding is UTF-8.

// Content characters "<", ">", """, "'" and "&" are automatically escaped when
// writing and resolved when reading. All other non-null characters are
// considered legal content.

// Illegal characters in names are replaced with underscores.
//------------------------------------------------------------------------------

#ifndef XML_h
#define XML_h
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
//------------------------------------------------------------------------------

#include "General.h"
#include "LLRBTree.h"
#include "Calculator.h"
//------------------------------------------------------------------------------

class Xml{
    public:
        struct Attribute{
            std::string name;
            std::string value;
            std::string legalValue;

            Attribute(const char* name, const char* value);
           ~Attribute();
        };

        struct Entity{
            std::string name;
            std::string comments;
            std::string content;
            std::string legalContent;

            LLRBTree children;   // Child entities, sorted by name
            LLRBTree attributes; // Sorted by name

            Entity(const char* name);
           ~Entity();
        };
        Entity* root; // When changing externally, call Clear() first

    private:
        struct Nesting{
            Entity*  entity;
            Nesting* next;

            Nesting(const char* EntityName);
           ~Nesting();
        };
        Nesting* nesting;

        std::string buffer;
        void getLegalName   (const char * name   , std::string* legalName   );
        void getLegalContent(std::string* content, std::string* legalContent);
        void saveEntity     (Entity     * entity , unsigned     indent = 0  );

        Calculator calc;

        unsigned readSize;
        unsigned readIndex;
        char*    readBuffer;

        void printError(const char* message);

        bool    readSpace    ();
        bool    readComment  ();
        bool    readSpecial  ();
        bool    readName     (std::string* buffer);
        bool    readContent  (std::string* buffer, char end = 0);
        bool    readAttribute(LLRBTree   * tree);
        bool    readHeader   ();
        Entity* readEntity   ();

    public:
        Xml();
       ~Xml();

        // Discard all previous data
        void clear();

        // Discard all previous data and creates a new document,
        // with a top entity named Document
        void newDocument(const char* document);

        // Nests a new entity under the current one
        void begin(const char* entity);

        // Adds a new comment to the current entity
        void comment(const char* comment);

        // Adds a new attribute to the current entity
        void attribute(const char* name, int          value); // Decimal
        void attribute(const char* name, bool         value);
        void attribute(const char* name, double       value);
        void attribute(const char* name, unsigned     value); // Hexadecimal
        void attribute(const char* name, const char*  value);
        void attribute(const char* name, std::string* value);

        // Adds to the existing content of the current entity
        void content(int         value);
        void content(bool        value);
        void content(double      value);
        void content(unsigned    value);
        void content(const char* value);

        // Closes the current entity and goes up one nesting level
        // Once the top level entity is closed, no other operations are possible
        void end();

        // Closes all entities on the nesting stack and saves the document to a file
        bool save(const char* filename);

        // Discards all previous data and loads the file into the current document
        bool load(const char* filename);

        // Finds the entity with name Name (Memory managed by this class)
        Entity* findChild(Entity* entity, const char* name);
        // After calling FindChild, call this to get the next duplicate
        Entity* nextChild(Entity* entity, const char* name);

        // Finds the attribute with name Name (Memory managed by this class)
        Attribute* findAttribute(Entity* entity, const char* name);

        // Use these to read attributes directly
        bool readAttribute(Entity* entity, const char* name, int*         value);
        bool readAttribute(Entity* entity, const char* name, bool*        value);
        bool readAttribute(Entity* entity, const char* name, char*        value);
        bool readAttribute(Entity* entity, const char* name, std::string* value);
        bool readAttribute(Entity* entity, const char* name, double*      value);
        bool readAttribute(Entity* entity, const char* name, unsigned*    value);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
