/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#include "test.h"
#include "Dictionary.h"
//------------------------------------------------------------------------------

DICTIONARY<int> Dictionary;
//------------------------------------------------------------------------------

// The default duplicate-handler replaces the value, but we actually want to
// count the number of instances...
static int* OnDuplicate(const char* Name, int* Old, int* New){
  delete New;
  (*Old) ++;
  return Old;
}
static void DisplayAction(const char* Name, int* Data){
  info("\"%s\" occurs %2d times", Name, *Data);
}

//------------------------------------------------------------------------------

// Some random words from https://www.lipsum.com/
const char* LoremIpsum[] = {
  "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipiscing", "elit",
  "Etiam", "vel", "eleifend", "dolor", "Quisque", "luctus", "eros", "mi", "et",
  "consectetur", "metus", "sodales", "vel", "Vestibulum", "ante", "ipsum",
  "primis", "in", "faucibus", "orci", "luctus", "et", "ultrices", "posuere", 
  "cubilia", "Curae", "Mauris", "molestie", "suscipit", "massa", "in", 
  "sodales", "Aenean", "varius", "dolor", "sapien", "a", "vulputate", "magna", 
  "aliquet", "vel", "Nam", "non", "augue", "massa", "Mauris", "ut", "mi", 
  "tellus", "Integer", "at", "varius", "mi", "sed", "condimentum", "dui", 
  "Ut", "ac", "tempor", "nisl", "Nulla", "semper", "elit", "eu", "massa", 
  "convallis", "pulvinar", "Nunc", "eu", "eros", "egestas", "metus", 
  "maximus", "rhoncus", "Maecenas", "consectetur", "maximus", "tortor", "non", 
  "auctor", "nisi", "eleifend", "quis", "Aliquam", "sapien", "massa", 
  "sodales", "et", "congue", "semper", "vulputate", "nec", "arcu", 
  "Vestibulum", "feugiat", "lacus", "ornare", "neque", "viverra", "varius", 
  "eget", "sit", "amet", "dui", "Ut", "vitae", "orci", "id", "dolor", 
  "cursus", "placerat", "vehicula", "in", "lectus", "Vestibulum", "efficitur", 
  "ullamcorper", "odio", "a", "iaculis", "massa", "dictum", "quis", "Nullam", 
  "convallis", "nisi", "a", "lobortis", "posuere", "Suspendisse", "lobortis", 
  "porta", "ullamcorper", "Curabitur", "rhoncus", "mi", "sit", "amet", "nisi", 
  "convallis", "interdum", "Vivamus", "molestie", "ac", "felis", "ac", 
  "fringilla", "Aenean", "fermentum", "ex", "vel", "elit", "sollicitudin", 
  "non", "ullamcorper", "nisi", "condimentum", "Donec", "ullamcorper", 
  "risus", "nisi", "ac", "rutrum", "ipsum", "aliquet", "vel", "Curabitur", 
  "luctus", "mollis", "volutpat", "Aliquam", "a", "purus", "efficitur", 
  "interdum", "urna", "quis", "commodo", "est", "Nulla", "dui", "enim", 
  "venenatis", "in", "feugiat", "eu", "semper", "id", "enim", "Nunc", "nec", 
  "lorem", "vel", "nunc", "imperdiet", "ultricies", "eget", "eget", "mi", 
  "Aenean", "nec", "lorem", "metus", "Etiam", "mollis", "nulla", "quis", 
  "massa", "cursus", "sagittis", "Fusce", "tempor", "eros", "lorem", "a", 
  "porta", "ipsum", "mollis", "sed", "Mauris", "sed", "lacus", "et", "erat", 
  "vehicula", "faucibus", "Pellentesque", "at", "ligula", "et", "tortor", 
  "vehicula", "aliquet", "quis", "id", "nibh", "Aenean", "facilisis", "quam", 
  "sit", "amet", "placerat", "feugiat", "purus", "augue", "ultricies", 
  "magna", "sed", "varius", "ex", "nunc", "vitae", "justo", "In", "hac", 
  "habitasse", "platea", "dictumst", "Integer", "ullamcorper", "mi", "ex", 
  "vitae", "pulvinar", "nulla", "lacinia", "sit", "amet", "Curabitur", "ut", 
  "lectus", "porta", "sagittis", "sapien", "at", "elementum", "dolor", "Duis", 
  "aliquet", "iaculis", "scelerisque", "Mauris", "sit", "amet", "ultricies", 
  "erat", "Mauris", "tincidunt", "non", "orci", "eget", "porttitor", "Nam", 
  "cursus", "mauris", "condimentum", "mauris", "imperdiet", "tempus", 
  "Quisque", "finibus", "accumsan", "vulputate", "Proin", "at", "felis", 
  "nec", "elit", "congue", "rhoncus", "sed", "iaculis", "ligula", "Integer", 
  "ut", "luctus", "quam", "Proin", "sed", "leo", "ligula", "Sed", "sed", 
  "tortor", "non", "libero", "tempor", "ullamcorper", "Etiam", "ornare", 
  "quam", "nec", "urna", "fermentum", "vel", "fringilla", "dolor", "sodales", 
  "Morbi", "tempor", "nulla", "sapien", "sed", "vulputate", "lacus", 
  "efficitur", "eu", "Mauris", "feugiat", "lectus", "ut", "malesuada", 
  "lacinia", "massa", "neque", "luctus", "eros", "ut", "hendrerit", "nunc", 
  "lacus", "sed", "nulla", "Aenean", "suscipit", "sapien", "eget", "lobortis", 
  "placerat", "quam", "enim", "elementum", "nisl", "faucibus", "vulputate", 
  "massa", "est", "mollis", "leo", "Phasellus", "eget", "justo", "rhoncus", 
  "ornare", "nunc", "non", "convallis", "felis", "Duis", "mattis", "ligula", 
  "vitae", "rutrum", "consequat", "justo", "nisi", "tincidunt", "dui", "eget", 
  "feugiat", "felis", "ipsum", "vitae", "purus", "Nullam", "nibh", "nibh", 
  "condimentum", "id", "porttitor", "in", "lacinia", "et", "elit", "Donec", 
  "cursus", "dui", "at", "nisl", "tincidunt", "posuere", "Interdum", "et", 
  "malesuada", "fames", "ac", "ante", "ipsum", "primis", "in", "faucibus", 
  "Quisque", "consequat", "diam", "id", "aliquet", "eleifend", "Donec", "vel", 
  "felis", "vitae", "tortor", "consectetur", "consequat", "sit", "amet", "in", 
  "risus", "Integer", "eget", "rutrum", "arcu", "Quisque", "mollis", "risus", 
  "vel", "leo", "scelerisque", "quis", "euismod", "elit", "pretium", "Duis", 
  "pharetra", "pulvinar", "rhoncus", "In", "ullamcorper", "metus", "non", 
  "vulputate", "dapibus", "massa", "nisl", "auctor", "lorem", "ut", 
  "pellentesque", "ex", "est", "vel", "nulla", "Cras", "egestas", "ornare", 
  "erat", "sed", "faucibus", "Sed", "commodo", "purus", "porta", "fringilla", 
  "lobortis", "nisl", "ligula", "auctor", "urna", "tempor", "feugiat", "mi", 
  "ligula", "fermentum", "ligula", "In", "mi", "nunc", "blandit", "sit", 
  "amet", "orci", "ac", "pharetra", "tincidunt", "nulla",
  0
};
//------------------------------------------------------------------------------

bool TestBuild(){
  Start("Testing Dictionary");

  Dictionary.OnDuplicate = (DICTIONARY_BASE::ON_DUPLICATE)OnDuplicate;

  int n;
  for(n = 0; LoremIpsum[n]; n++){
    int* Counter = new int;
    *Counter = 1;
    Dictionary.Insert(LoremIpsum[n], Counter);
  }

  info("The tree now contains %2d items", Dictionary.GetCount());
  Assert(Dictionary.GetCount() == 160);

  int VestibulumCount = *Dictionary.Find("Vestibulum");
  info("Finding \"Vestibulum\"...  It occurs %2d times.", VestibulumCount);
  Assert(VestibulumCount == 3);

  Done(); return true;
}
//------------------------------------------------------------------------------

bool TestIteration(){
  Start("Testing Dictionary Action");
    Dictionary.Action((DICTIONARY_BASE::ACTION)DisplayAction);
  Done();

  Start("Testing Dictionary Iteration");

  const char* Name;
  int* Count = Dictionary.First(&Name);
  while(Count){
    info("\"%s\" occurs %2d times", Name, *Count);
    Count = Dictionary.Next(&Name);
  }

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  SetupTerminal();

  printf("\n\n");
  if(!TestBuild    ()) goto main_Error;
  if(!TestIteration()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors");
    return -1;
}
//------------------------------------------------------------------------------

