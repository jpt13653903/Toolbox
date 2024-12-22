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

class DictionaryBase{
    public:
        typedef void  (*Action     )(const char* name, void* data);
        typedef void* (*OnDuplicate)(const char* name, void* oldNode, void* newNode);

    private:
        struct Node{
            bool  red;

            char* name; ///< This memory is internally managed
            void* data; ///< This is arbitrary data and not deleted by this structure

            // Used to point to the children
            Node* left;
            Node* right;

            // Used to linearise the tree
            Node* prev;
            Node* next;

            Node(const char* name, void* data, Node* prev, Node* next);
           ~Node();
        };

        Node* root;
        Node* current; // Used in "First" and "Next" calls
        Node* tempPrev;
        Node* tempNext;
        int   itemCount;

        bool  isRed      (Node* node);
        void  colourFlip (Node* node);
        Node* rotateLeft (Node* node);
        Node* rotateRight(Node* node);

        Node* insert(Node* node, const char* name, void* data);
        void  action(Node* node, Action function);

        static void* defaultOnDuplicate(const char* name, void* oldNode, void* newNode);

    protected:
        void  clear ();
        void  insert(const char*  name, void* data);
        void* find  (const char*  name);
        void* first (const char** name = 0);
        void* next  (const char** name = 0);

    public:
                 DictionaryBase();
        virtual ~DictionaryBase();

        // Callback function called upon duplicate insert.  The return value must
        // be the data that must be stored at that location.  The default behaviour
        // is to update to the new data, without calling "delete" or similar.
        //
        // Note: The template class below does call "delete"
        OnDuplicate onDuplicate;

        int getCount();

        // This calls the given function for every node, in order
        void action(Action function);
};
//------------------------------------------------------------------------------

template<class type> class Dictionary : public DictionaryBase{
    private:
        static type* defaultOnDuplicate(const char* name, type* oldNode, type* newNode){
            delete oldNode;
            return newNode;
        }
        static void cleanupAction(const char* name, type* data){
            delete data;
        }

    public:
        Dictionary(){
            onDuplicate = (OnDuplicate)defaultOnDuplicate;
        }
        virtual ~Dictionary(){
            action((Action)cleanupAction);
        }
        void clear(){
            action((Action)cleanupAction);
            DictionaryBase::clear();
        }
        void insert(const char* name, type* data){
            DictionaryBase::insert(name, data);
        }
        type* find(const char* name){
            return (type*)DictionaryBase::find(name);
        }
        type* first(const char** name = 0){
            return (type*)DictionaryBase::first(name);
        }
        type* next(const char** name = 0){
            return (type*)DictionaryBase::next(name);
        }
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

