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

#include "test.h"
#include "Calculator.h"
//------------------------------------------------------------------------------

static CALCULATOR Calc;
//------------------------------------------------------------------------------

bool Calculate(const char* Expression, double Expected){
  double Result = Calc.Calculate(Expression);
  info("  %s = %lg", Expression, Result);
  Assert(Result == Expected);
  return true;
}
//------------------------------------------------------------------------------

bool TestConstants(){
  Start("Constants");

  if(!Calculate("e"        , 2.718281828459045235360)) return false;
  if(!Calculate("pi"       , 3.141592653589793238463)) return false;
  if(!Calculate("\\c"      , 2.99792458e8           )) return false;
  if(!Calculate("\\G"      , 6.67259e-11            )) return false;
  if(!Calculate("\\g"      , 9.80665                )) return false;
  if(!Calculate("\\me"     , 9.1093897e-31          )) return false;
  if(!Calculate("\\mp"     , 1.6726231e-27          )) return false;
  if(!Calculate("\\mn"     , 1.6749286e-27          )) return false;
  if(!Calculate("\\u"      , 1.6605402e-27          )) return false;
  if(!Calculate("\\e"      , 1.60217646e-19         )) return false;
  if(!Calculate("\\h"      , 6.6260689633e-34       )) return false;
  if(!Calculate("\\k"      , 1.380650424e-23        )) return false;
  if(!Calculate("\\mu"     , 1.2566370614359173e-6  )) return false;
  if(!Calculate("\\epsilon", 8.854187817e-12        )) return false;
  if(!Calculate("\\Phi"    , 2.06783461e-15         )) return false;
  if(!Calculate("\\NA"     , 6.0221417930e23        )) return false;
  if(!Calculate("\\R"      , 8.31447215             )) return false;
  if(!Calculate("\\rhoAl"  , 26.53e-9               )) return false;
  if(!Calculate("\\rhoCu"  , 16.78e-9               )) return false;
  if(!Calculate("\\F"      , 9.6485304e4            )) return false;
  if(!Calculate("\\AU"     , 1.4959787e11           )) return false;
  if(!Calculate("\\pc"     , 3.0856776e16           )) return false;
  if(!Calculate("\\mil"    , 25.4e-6                )) return false;

  Done(); return true;
}
//------------------------------------------------------------------------------

bool TestExpressions(){
  Start("Expressions");

  if(!Calculate(" 3 + 4  * 5", 23)) return false;
  if(!Calculate("(3 + 4) * 5", 35)) return false;
  if(!Calculate("sin(log(e^pi))", 978.770937703933023e-3)) return false;

  Done(); return true;
}
//------------------------------------------------------------------------------

void TreeView(const char* Expression){
  std::string Tree;
  Calc.BuildTree(Expression);
  Calc.ShowTree (&Tree);
  info("%s\n%s", Expression, Tree.c_str());
}
//------------------------------------------------------------------------------

bool TestTreeView(){
  Start("Viewing the tree");

  TreeView(" 3 + x  * 5");
  TreeView("(3 + x) * 5");
  TreeView("sin(log(e^(pi + x)))");
  TreeView("d(sin(log(e^(pi + x))))dx");

  Done(); return true;
}
//------------------------------------------------------------------------------

int main(){
  printf("\n\n");
  if(!TestConstants  ()) goto main_Error;
  if(!TestExpressions()) goto main_Error;
  if(!TestTreeView   ()) goto main_Error;

  info(ANSI_FG_GREEN "All OK"); Done();
  return 0;

  main_Error:
    Done(); info(ANSI_FG_BRIGHT_RED "There were errors"); Done();
    return -1;
}
//------------------------------------------------------------------------------

