/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
--------------------------------------------------------------------------------

This is a dictionary to store arbitrary data with a string ID (also known as 
an associative array).  It is based on a left-leaning red-black tree, as 
described by Robert Sedgewick, Department of Computer Science, Princeton 
University, Princeton, NJ 08544
==============================================================================*/

#ifndef Dictionary_h
#define Dictionary_h
//------------------------------------------------------------------------------

class DICTIONARY{
  public:
    typedef void  (*DICTIONARY_ACTION   )(const char* Name, void* Data);
    typedef void* (*DICTIONARY_DUPLICATE)(const char* Name, void* Old, void* New);

  private:
    struct NODE{
      bool  Red;

      char* Name; ///< This memory is internally managed
      void* Data; ///< This is arbitrary data and not deleted by this structure

      NODE* Left;
      NODE* Right;

      NODE(const char* Name, void* Data);
     ~NODE();
    };

    NODE* Root;
    int   ItemCount;

    bool  IsRed      (NODE* Node);
    void  ColourFlip (NODE* Node);
    NODE* RotateLeft (NODE* Node);
    NODE* RotateRight(NODE* Node);

    NODE* Insert(NODE* Node, const char* Name, void* Data);
    void  Action(NODE* Node, DICTIONARY_ACTION Function);

    static void* DefaultOnDuplicate(const char* Name, void* Old, void* New);

  public:
             DICTIONARY();
    virtual ~DICTIONARY();

    // Callback function called upon duplicate insert.  The return value must
    // be the data that must be stored at that location.  The default behaviour
    // is to update to the new data, without calling "delete" or similar.
    DICTIONARY_DUPLICATE OnDuplicate;

    void  Clear   ();
    void  Insert  (const char* Name, void* Data);
    void* Find    (const char* Name);
    int   GetCount();

    // This calls the given function for every node, in order
    void Action(DICTIONARY_ACTION Function);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

