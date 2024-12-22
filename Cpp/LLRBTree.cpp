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

#include "LLRBTree.h"
//------------------------------------------------------------------------------

static int defaultCompare(void* left, void* right)
{
    if(left < right) return -1;
    if(left > right) return  1;
    return 0;
}
//------------------------------------------------------------------------------

LLRBTree::Node::Node(void* data, Node* next, Node* prev, int tag)
{
    left = right = 0;
    colour       = true;
    this->data   = data;
    this->next   = next;
    this->prev   = prev;
    this->tag    = tag;

    if(next) next->prev = this;
    if(prev) prev->next = this;
}
//------------------------------------------------------------------------------

LLRBTree::Node::~Node()
{
    if(next) next->prev = prev;
    if(prev) prev->next = next;

    if(left ) delete left;
    if(right) delete right;
}
//------------------------------------------------------------------------------

LLRBTree::LLRBTree()
{
    root         = 0;
    currentNode  = 0;
    theItemCount = 0;
    compare      = defaultCompare;
}
//------------------------------------------------------------------------------

LLRBTree::~LLRBTree()
{
    clear();
}
//------------------------------------------------------------------------------

bool LLRBTree::isRed(Node* node)
{
    if(node) return node->colour;
    return false;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::fixUp(Node* node)
{
    if(isRed(node->right) && !isRed(node->left      )) node = rotateLeft (node);
    if(isRed(node->left ) &&  isRed(node->left->left)) node = rotateRight(node);
    if(isRed(node->left ) &&  isRed(node->right     ))        flipColours(node);

    return node;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::rotateLeft(Node* node)
{
    Node* temp   = node->right;
    node->right  = temp->left;
    temp->left   = node;
    temp->colour = node->colour;
    node->colour = true;

    return temp;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::rotateRight(Node* node)
{
    Node* temp   = node->left;
    node->left   = temp->right;
    temp->right  = node;
    temp->colour = node->colour;
    node->colour = true;

    return temp;
}
//------------------------------------------------------------------------------

void LLRBTree::flipColours(Node* node)
{
    node       ->colour = !node       ->colour;
    node->left ->colour = !node->left ->colour;
    node->right->colour = !node->right->colour;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::insert(Node* node, void* data, int tag)
{
    if(!node){
        theItemCount++;
        return new Node(data, tempNext, tempPrev, tag);
    }

    int result = compare(data, node->data);

    if(!result){ // Duplicate
        tag    = node->tag + 1; // It is "larger" than the duplicate in the tree
        result = 1;             // and therefore to the right of it
    }

    if(result < 0){
        tempNext = node;
        node->left  = insert(node->left , data, tag);

    }else{
        tempPrev = node;
        node->right = insert(node->right, data, tag);
    }

    return fixUp(node);
}
//------------------------------------------------------------------------------

void LLRBTree::insert(void* data)
{
    tempNext = tempPrev = 0;

    root = insert(root, data);
    root->colour = false;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::moveRedLeft(Node* node)
{
    flipColours(node);

    if(isRed(node->right->left)){
        node->right = rotateRight(node->right);
        node        = rotateLeft (node);
        flipColours(node);
    }

    return node;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::moveRedRight(Node* node)
{
    flipColours(node);

    if(isRed(node->left->left)){
        node = rotateRight(node);
        flipColours(node);
    }

    return node;
}
//------------------------------------------------------------------------------

int LLRBTree::removeCompare(void* left, void* right, int tagLeft, int tagRight)
{
    int result;

    result = compare(left, right);
    if(!result){
        if(tagLeft < tagRight) return -1;
        if(tagLeft > tagRight) return  1;
        return 0;
    }
    return result;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::removeMin(Node* node)
{
    if(!node->left){
        if(currentNode == node) currentNode = 0;
        theItemCount--;
        delete node;
        return 0;
    }

    if(!isRed(node->left) && !isRed(node->left->left)) node = moveRedLeft(node);
    node->left = removeMin(node->left);

    return fixUp(node);
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::remove(Node* node, void* key, int tag)
{
    Node* temp;

    if(!node) return 0;

    if(removeCompare(key, node->data, tag, node->tag) < 0){
        if(!node->left) return node; // Key not in tree
        if(!isRed(node->left) && !isRed(node->left->left)) node = moveRedLeft(node);
        node->left = remove(node->left, key, tag);

    }else{
        if(isRed(node->left)) node = rotateRight(node);

        // If N->Right is null, N is always a red leaf;
        if(!removeCompare(key, node->data, tag, node->tag) && !node->right){
            if(currentNode == node) currentNode = 0;
            theItemCount--;
            delete node;
            return 0;
        }

        if(!node->right) return fixUp(node); // Key not in tree

        if(!isRed(node->right) && !isRed(node->right->left)) node = moveRedRight(node);

        if(!removeCompare(key, node->data, tag, node->tag)){
            // Find the minimum in the right sub-tree
            temp = node->right;
            while(temp && temp->left) temp = temp->left;
            // Copy temp's data to this location and delete temp
            if(currentNode == temp) currentNode = node;
            node->data  = temp->data;
            node->right = removeMin(node->right);

        }else{
            node->right = remove(node->right, key, tag);
        }
    }

    return fixUp(node);
}
//------------------------------------------------------------------------------

void LLRBTree::remove(void* key)
{
    Node* temp;
    Node* node;

    // Find the first duplicate
    node = find(root, key);

    // Check if there is a duplicate storing the same pointer
    temp = node->next;
    while(temp){
        if(compare(key, temp->data)){ // No more duplicates
            temp = 0;
            break;
        }
        if(key == temp->data) break; // Found it
        temp = temp->next;
    }
    if(temp) node = temp;

    if(!node) return;

    root = remove(root, key, node->tag);
    if(root) root->colour = false;
}
//------------------------------------------------------------------------------

LLRBTree::Node* LLRBTree::find(Node* node, void* key)
{
    int   result;
    Node* temp = 0;

    while(node){
        result = compare(key, node->data);
        if     (result < 0) node = node->left;
        else if(result > 0) node = node->right;
        else{
            temp = node;       // Store the current "best match" and
            node = node->left; // Carry on searching the left sub-tree
        }
    }
    return temp;
}
//------------------------------------------------------------------------------

void* LLRBTree::find(void* key)
{
    currentNode = find(root, key);
    if(currentNode) return currentNode->data;

    return 0;
}
//------------------------------------------------------------------------------

void* LLRBTree::before(void* key)
{
    int   result;
    Node* node;

    currentNode = 0;

    node = root;
    while(node){
        result = compare(key, node->data);
        if(result > 0){
            currentNode = node;
            node        = node->right;
        }else{
            node        = node->left;
        }
    }

    if(currentNode) return currentNode->data;
    return 0;
}
//------------------------------------------------------------------------------

void* LLRBTree::after(void* key)
{
    int   result;
    Node* node;

    currentNode = 0;

    node = root;
    while(node){
        result = compare(key, node->data);
        if(result < 0){
            currentNode = node;
            node        = node->left;
        }else{
            node        = node->right;
        }
    }

    if(currentNode) return currentNode->data;
    return 0;
}
//------------------------------------------------------------------------------

void* LLRBTree::first()
{
    if(!root) return 0;

    currentNode = root;
    while(currentNode && currentNode->left) currentNode = currentNode->left;

    return currentNode->data;
}
//------------------------------------------------------------------------------

void* LLRBTree::last()
{
    if(!root) return 0;

    currentNode = root;
    while(currentNode && currentNode->right) currentNode = currentNode->right;

    return currentNode->data;
}
//------------------------------------------------------------------------------

void* LLRBTree::next()
{
    if(currentNode) currentNode = currentNode->next;
    if(currentNode) return        currentNode->data;
    return 0;
}
//------------------------------------------------------------------------------

void* LLRBTree::previous()
{
    if(currentNode) currentNode = currentNode->prev;
    if(currentNode) return        currentNode->data;
    return 0;
}
//------------------------------------------------------------------------------

void* LLRBTree::current()
{
    if(currentNode) return currentNode->data;
    return 0;
}
//------------------------------------------------------------------------------

void* LLRBTree::rootItem()
{
    currentNode = root;
    if(currentNode) return currentNode->data;
    return 0;
}
//------------------------------------------------------------------------------

void LLRBTree::clear()
{
    if(root) delete root;
    root         = 0;
    currentNode  = 0;
    theItemCount = 0;
}
//------------------------------------------------------------------------------

unsigned LLRBTree::itemCount()
{
    return theItemCount;
}
//------------------------------------------------------------------------------

