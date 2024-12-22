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

#ifndef LLRBTree_h
#define LLRBTree_h
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stddef.h>
//------------------------------------------------------------------------------

// Should return:
// -1 if Left key < Right key
//  0 if Left key = Right key
//  1 if Left key > Right key
// For find and remove, Left is the key and Right the node
typedef int (*LLRBTreeCompare)(void* left, void* right);
//------------------------------------------------------------------------------

/**
The data is stored in a Left-Leaning Red-Black Tree, as described by
Robert Sedgewick, Department of Computer Science, Princeton University,
Princeton, NJ 08544
*/
class LLRBTree{
    private:
        struct Node{
            Node* left;
            Node* right;
            bool  colour; // true => Red
            int   tag;    // Keeps track of insertion order of duplicates
            void* data;

            // Double linked list maintained for quick Next() Prev() calls
            Node* next;
            Node* prev;

            Node(void* data, Node* next, Node* prev, int tag);
           ~Node();
        };
        Node* root;
        Node* currentNode;

        Node* tempNext; // Used while inserting to find the correct place
        Node* tempPrev; // in the linked list to place the new item

        unsigned theItemCount;

        inline bool  isRed(Node* node);
        inline Node* fixUp(Node* node);

        // Insert functions
        Node* insert     (Node* node, void* data, int tag = 0);
        Node* rotateLeft (Node* node);
        Node* rotateRight(Node* node);
        void  flipColours(Node* node);

        // Remove functions
        int   removeCompare(void* left, void* right, int tagLeft, int tagRight);
        Node* remove       (Node* node, void* key,   int tag);
        Node* moveRedLeft  (Node* node);
        Node* moveRedRight (Node* node);
        Node* removeMin    (Node* node);

        // Find function
        Node* find(Node* node, void* key);
//------------------------------------------------------------------------------

    public:
        LLRBTree();
       ~LLRBTree();

        void  insert(void* data); // Adds "Data" to the tree. Duplicates are sorted
                                  // by order of insertion.
        void  remove(void* key ); // Removes the item with the same key as "Key"
                                  // In the case of duplicates: if Key is in the tree
                                  // under the same pointer, removes that one,
                                  // otherwise removes the first duplicate found.
        void* find  (void* key ); // Returns the item with the same key as "Key",
                                  // null if the item is not found.  Returns the
                                  // first of the list of duplicates.  Also updates
                                  // "Current"
        void* before(void* key ); // Returns the item just before "Key" and
                                  // updates "Current"
        void* after (void* key ); // Returns the item just after "Key" and
                                  // updates "Current"
        void* first(); // Returns the item with the smallest key and updates "Current"
        void* last (); // Returns the item with the largest key and updates "Current"
        void* next (); // Returns the next item and updates "Current";
                       // null if "Current" is the last item (or null)
        void* previous(); // Returns the previous item and updates "Current";
                          // null if "Current" is the first item (or null)
        void* current();  // Returns the current item, null if "Current" is null
        void* rootItem(); // Returns the item at the root of the tree, which is
                          // approximately the median, and updates "Current"
        void  clear();    // Clears the data;

        unsigned itemCount();

        LLRBTreeCompare compare;
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
