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

#include "clLLRBTree.h"
//------------------------------------------------------------------------------

static int DefaultCompare(void* Left, void* Right){
  if(Left < Right) return -1;
  if(Left > Right) return  1;
  return 0;
}
//------------------------------------------------------------------------------

clLLRBTree::Node::Node(void* Data, Node* Next, Node* Prev, int Tag){
  Left = Right = 0;
  Colour       = true;
  this->Data   = Data;
  this->Next   = Next;
  this->Prev   = Prev;
  this->Tag    = Tag;

  if(Next) Next->Prev = this;
  if(Prev) Prev->Next = this;
}
//------------------------------------------------------------------------------

clLLRBTree::Node::~Node(){
  if(Left ) delete Left;
  if(Right) delete Right;

  if(Next) Next->Prev = Prev;
  if(Prev) Prev->Next = Next;
}
//------------------------------------------------------------------------------

clLLRBTree::clLLRBTree(){
  Root         = 0;
  CurrentNode  = 0;
  TheItemCount = 0;
  Compare      = DefaultCompare;
}
//------------------------------------------------------------------------------

clLLRBTree::~clLLRBTree(){
  Clear();
}
//------------------------------------------------------------------------------

bool clLLRBTree::IsRed(Node* N){
  if(N) return N->Colour;
  return false;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::FixUp(Node* N){
  if(IsRed(N->Right) && !IsRed(N->Left      )) N = RotateLeft (N);
  if(IsRed(N->Left ) &&  IsRed(N->Left->Left)) N = RotateRight(N);
  if(IsRed(N->Left ) &&  IsRed(N->Right     ))     FlipColours(N);

  return N;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::RotateLeft(Node* N){
  Node* t   = N->Right;
  N->Right  = t->Left;
  t->Left   = N;
  t->Colour = N->Colour;
  N->Colour = true;

  return t;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::RotateRight(Node* N){
  Node* t   = N->Left;
  N->Left   = t->Right;
  t->Right  = N;
  t->Colour = N->Colour;
  N->Colour = true;

  return t;
}
//------------------------------------------------------------------------------

void clLLRBTree::FlipColours(Node* N){
  N       ->Colour = !N       ->Colour;
  N->Left ->Colour = !N->Left ->Colour;
  N->Right->Colour = !N->Right->Colour;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::Insert(Node* N, void* Data, int Tag){
  if(!N){
    TheItemCount++;
    return new Node(Data, TempNext, TempPrev, Tag);
  }

  int result = Compare(Data, N->Data);
  
  if(!result){ // Duplicate
    Tag = N->Tag + 1; // It is "larger" than the duplicate in the tree
    result = 1;       // and therefore to the right of it
  }

  if(result < 0){
    TempNext = N;
    N->Left  = Insert(N->Left , Data, Tag);

  }else{
    TempPrev = N;
    N->Right = Insert(N->Right, Data, Tag);
  }
 
  return FixUp(N);
}
//------------------------------------------------------------------------------

void clLLRBTree::Insert(void* Data){
  TempNext = TempPrev = 0;

  Root = Insert(Root, Data);
  Root->Colour = false;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::MoveRedLeft(Node* N){
  FlipColours(N);

  if(IsRed(N->Right->Left)){
    N->Right = RotateRight(N->Right);
    N        = RotateLeft (N);
    FlipColours(N);
  }

  return N;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::MoveRedRight(Node* N){
  FlipColours(N);

  if(IsRed(N->Left->Left)){
    N = RotateRight(N);
    FlipColours(N);
  }

  return N;
}
//------------------------------------------------------------------------------

int clLLRBTree::RemoveCompare(void* Left, void* Right, int TagLeft, int TagRight){
  int result;

  result = Compare(Left, Right);
  if(!result){
    if(TagLeft < TagRight) return -1;
    if(TagLeft > TagRight) return  1;
    return 0;
  }
  return result;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::RemoveMin(Node* N){
  if(!N->Left){
    if(CurrentNode == N) CurrentNode = 0;
    TheItemCount--;
    delete N;
    return 0;
  }

  if(!IsRed(N->Left) && !IsRed(N->Left->Left)) N = MoveRedLeft(N);
  N->Left = RemoveMin(N->Left);

  return FixUp(N);
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::Remove(Node* N, void* Key, int Tag){
  Node* Temp;

  if(!N) return 0;

  if(RemoveCompare(Key, N->Data, Tag, N->Tag) < 0){
    if(!N->Left) return N; // Key not in tree
    if(!IsRed(N->Left) && !IsRed(N->Left->Left)) N = MoveRedLeft(N);
    N->Left = Remove(N->Left, Key, Tag);

  }else{
    if(IsRed(N->Left)) N = RotateRight(N);

    // If N->Right is null, N is always a red leaf;
    if(!RemoveCompare(Key, N->Data, Tag, N->Tag) && !N->Right){
      if(CurrentNode == N) CurrentNode = 0;
      TheItemCount--;
      delete N;
      return 0;
    }

    if(!N->Right) return FixUp(N); // Key not in tree

    if(!IsRed(N->Right) && !IsRed(N->Right->Left)) N = MoveRedRight(N);

    if(!RemoveCompare(Key, N->Data, Tag, N->Tag)){
      // Find the minimum in the right sub-tree
      Temp = N->Right;
      while(Temp && Temp->Left) Temp = Temp->Left;
      // Copy Temp's data to this location and delete Temp
      if(CurrentNode == Temp) CurrentNode = N;
      N->Data  = Temp->Data;
      N->Right = RemoveMin(N->Right);

    }else{
      N->Right = Remove(N->Right, Key, Tag);
    }
  }

  return FixUp(N);
}
//------------------------------------------------------------------------------

void clLLRBTree::Remove(void* Key){
  int   result;
  Node* Temp; 
  Node* N; 

  // Find the first duplicate
  N = Find(Root, Key);

  // Check if there is a duplicate storing the same pointer
  Temp = N->Next;
  while(Temp){
    if(Compare(Key, Temp->Data)){ // No more duplicates
      Temp = 0;
      break;
    }
    if(Key == Temp->Data) break; // Found it
    Temp = Temp->Next;
  }
  if(Temp) N = Temp;

  if(!N) return;

  Root = Remove(Root, Key, N->Tag);
  if(Root) Root->Colour = false;
}
//------------------------------------------------------------------------------

clLLRBTree::Node* clLLRBTree::Find(Node* N, void* Key){
  int   result;
  Node* Temp = 0;

  while(N){
    result = Compare(Key, N->Data);
    if     (result < 0) N = N->Left;
    else if(result > 0) N = N->Right;
    else{
      Temp = N;       // Store the current "best match" and
      N    = N->Left; // carry on searching the left sub-tree
    }
  }
  return Temp;
}
//------------------------------------------------------------------------------

void* clLLRBTree::Find(void* Key){
  CurrentNode = Find(Root, Key);
  if(CurrentNode) return CurrentNode->Data;

  return 0;
}
//------------------------------------------------------------------------------

void* clLLRBTree::Before(void* Key){
  int   result;
  Node* N;

  CurrentNode = 0;

  N = Root;
  while(N){
    result = Compare(Key, N->Data);
    if(result > 0){
      CurrentNode = N;
      N           = N->Right;
    }else{
      N           = N->Left;
    }
  }

  if(CurrentNode) return CurrentNode->Data;
  return 0;
}
//------------------------------------------------------------------------------

void* clLLRBTree::After(void* Key){
  int   result;
  Node* N;

  CurrentNode = 0;

  N = Root;
  while(N){
    result = Compare(Key, N->Data);
    if(result < 0){
      CurrentNode = N;
      N           = N->Left;
    }else{
      N           = N->Right;
    }
  }

  if(CurrentNode) return CurrentNode->Data;
  return 0;
}
//------------------------------------------------------------------------------

void* clLLRBTree::First(){
  if(!Root) return 0;

  CurrentNode = Root;
  while(CurrentNode && CurrentNode->Left) CurrentNode = CurrentNode->Left;

  return CurrentNode->Data;
}
//------------------------------------------------------------------------------

void* clLLRBTree::Last(){
  if(!Root) return 0;

  CurrentNode = Root;
  while(CurrentNode && CurrentNode->Right) CurrentNode = CurrentNode->Right;

  return CurrentNode->Data;
}
//------------------------------------------------------------------------------

void* clLLRBTree::Next(){
  if(CurrentNode) CurrentNode = CurrentNode->Next;
  if(CurrentNode) return        CurrentNode->Data;
  return 0;
}
//------------------------------------------------------------------------------

void* clLLRBTree::Previous(){
  if(CurrentNode) CurrentNode = CurrentNode->Prev;
  if(CurrentNode) return        CurrentNode->Data;
  return 0;
}
//------------------------------------------------------------------------------

void* clLLRBTree::Current(){
  if(CurrentNode) return CurrentNode->Data;
  return 0;
}
//------------------------------------------------------------------------------

void* clLLRBTree::RootItem(){
  CurrentNode = Root;
  if(CurrentNode) return CurrentNode->Data;
  return 0;
}
//------------------------------------------------------------------------------

void clLLRBTree::Clear(){
  if(Root) delete Root;
  Root         = 0;
  CurrentNode  = 0;
  TheItemCount = 0;
}
//------------------------------------------------------------------------------

unsigned clLLRBTree::ItemCount(){
  return TheItemCount;
}
//------------------------------------------------------------------------------

