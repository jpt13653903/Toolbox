/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#include "Dictionary.h"
//------------------------------------------------------------------------------

DictionaryBase::Node::Node(const char* name, void* data, Node* prev, Node* next)
{
    red  = true;
    left = right = 0;

    int j;
    for(j = 0; name[j]; j++);
    this->name = new char[j+1];
    for(j = 0; name[j]; j++) this->name[j] = name[j];
    this->name[j] = 0;

    this->data = data;
    this->prev = prev;
    this->next = next;

    if(prev) prev->next = this;
    if(next) next->prev = this;
}
//------------------------------------------------------------------------------

DictionaryBase::Node::~Node()
{
    if(prev) prev->next = next;
    if(next) next->prev = prev;

    delete[] name;

    if(left ) delete left;
    if(right) delete right;
}
//------------------------------------------------------------------------------

void* DictionaryBase::defaultOnDuplicate(const char* name, void* oldNode, void* newNode)
{
    return newNode;
}
//------------------------------------------------------------------------------

DictionaryBase::DictionaryBase()
{
    root        = 0;
    current     = 0;
    itemCount   = 0;
    onDuplicate = defaultOnDuplicate;
}
//------------------------------------------------------------------------------

DictionaryBase::~DictionaryBase()
{
    if(root) delete root;
}
//------------------------------------------------------------------------------

void DictionaryBase::clear()
{
    if(root) delete root;
    root      = 0;
    current   = 0;
    itemCount = 0;
}
//------------------------------------------------------------------------------

bool DictionaryBase::isRed(Node* node)
{
    if(!node) return false;
    return node->red;
}
//------------------------------------------------------------------------------

void DictionaryBase::colourFlip(Node* node)
{
    node       ->red = !node       ->red;
    node->left ->red = !node->left ->red;
    node->right->red = !node->right->red;
}
//------------------------------------------------------------------------------

DictionaryBase::Node* DictionaryBase::rotateLeft(Node* node)
{
    Node* temp  = node->right;
    node->right = temp->left;
    temp->left  = node;
    temp->red   = node->red;
    node->red   = true;
    return temp;
}
//------------------------------------------------------------------------------

DictionaryBase::Node* DictionaryBase::rotateRight(Node* node)
{
    Node* temp  = node->left;
    node->left  = temp->right;
    temp->right = node;
    temp->red   = node->red;
    node->red   = true;
    return temp;
}
//------------------------------------------------------------------------------

void DictionaryBase::insert(const char* name, void* data)
{
    tempPrev = tempNext = 0;
    root = insert(root, name, data);
}
//------------------------------------------------------------------------------

DictionaryBase::Node* DictionaryBase::insert(
    Node*       node,
    const char* name,
    void*       data
){
    if(!node){
        itemCount++;
        return new Node(name, data, tempPrev, tempNext);
    }

    int j;
    for(j = 0; name[j]; j++){
        if(name[j] < node->name[j]){
            tempNext   = node;
            node->left = insert(node->left, name, data);
            break;

        }else if(name[j] > node->name[j]){
            tempPrev    = node;
            node->right = insert(node->right, name, data);
            break;
        }
    }
    if(!name[j]){
        if(!node->name[j]){
            node->data = onDuplicate(name, node->data, data);
            return node;
        }else{
            tempNext   = node;
            node->left = insert(node->left, name, data);
        }
    }

    if(isRed(node->right) && !isRed(node->left      )) node = rotateLeft (node);
    if(isRed(node->left ) &&  isRed(node->left->left)) node = rotateRight(node);
    if(isRed(node->left ) &&  isRed(node->right     ))        colourFlip (node);

    return node;
}
//------------------------------------------------------------------------------

void* DictionaryBase::find(const char* name)
{
    int   j;
    Node* node = root;

    while(node){
        if(name[0] < node->name[0]){
            node = node->left;

        }else if(name[0] > node->name[0]){
            node = node->right;

        }else{
            for(j = 1; name[j]; j++){
                if(name[j] < node->name[j]){
                    node = node->left;
                    break;

                }else if(name[j] > node->name[j]){
                    node = node->right;
                    break;
                }
            }
            if(!name[j]){
                if(!node->name[j]){
                    current = node;
                    return node->data;
                }else{
                    node = node->left;
                }
            }
        }
    }
    return 0;
}
//------------------------------------------------------------------------------

void* DictionaryBase::first(const char** name)
{
    current = root;
    if(!current) return 0;
    while(current->left) current = current->left;
    if(name) *name = current->name;
    return current->data;
}
//------------------------------------------------------------------------------

void* DictionaryBase::next(const char** name)
{
    if(!current) return 0;
    current = current->next;
    if(!current) return 0;
    if(name) *name = current->name;
    return current->data;
}
//------------------------------------------------------------------------------

int DictionaryBase::getCount()
{
    return itemCount;
}
//------------------------------------------------------------------------------

void DictionaryBase::action(Action function)
{
    if(root) action(root, function);
}
//------------------------------------------------------------------------------

void DictionaryBase::action(Node* node, Action function)
{
    if(node->left ) action(node->left , function);
    function(node->name, node->data);
    if(node->right) action(node->right, function);
}
//------------------------------------------------------------------------------
