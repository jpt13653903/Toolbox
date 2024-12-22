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

static Calculator calc;
//------------------------------------------------------------------------------

bool calculate(const char* expression, double expected)
{
    double result = calc.calculate(expression);
    info("  %s = %g", expression, result);
    assert(result == expected, return false);
    return true;
}
//------------------------------------------------------------------------------

bool testConstants()
{
    start("Constants");

    if(!calculate("e"        , 2.718281828459045235360)) return false;
    if(!calculate("pi"       , 3.141592653589793238463)) return false;
    if(!calculate("\\c"      , 2.99792458e8           )) return false;
    if(!calculate("\\G"      , 6.67259e-11            )) return false;
    if(!calculate("\\g"      , 9.80665                )) return false;
    if(!calculate("\\me"     , 9.1093897e-31          )) return false;
    if(!calculate("\\mp"     , 1.6726231e-27          )) return false;
    if(!calculate("\\mn"     , 1.6749286e-27          )) return false;
    if(!calculate("\\u"      , 1.6605402e-27          )) return false;
    if(!calculate("\\e"      , 1.60217646e-19         )) return false;
    if(!calculate("\\h"      , 6.6260689633e-34       )) return false;
    if(!calculate("\\k"      , 1.380650424e-23        )) return false;
    if(!calculate("\\mu"     , 1.2566370614359173e-6  )) return false;
    if(!calculate("\\epsilon", 8.854187817e-12        )) return false;
    if(!calculate("\\Phi"    , 2.06783461e-15         )) return false;
    if(!calculate("\\NA"     , 6.0221417930e23        )) return false;
    if(!calculate("\\R"      , 8.31447215             )) return false;
    if(!calculate("\\rhoAl"  , 26.53e-9               )) return false;
    if(!calculate("\\rhoCu"  , 16.78e-9               )) return false;
    if(!calculate("\\F"      , 9.6485304e4            )) return false;
    if(!calculate("\\AU"     , 1.4959787e11           )) return false;
    if(!calculate("\\pc"     , 3.0856776e16           )) return false;
    if(!calculate("\\mil"    , 25.4e-6                )) return false;

    done(); return true;
}
//------------------------------------------------------------------------------

bool testExpressions()
{
    start("Expressions");

    if(!calculate(" 3 + 4  * 5", 23)) return false;
    if(!calculate("(3 + 4) * 5", 35)) return false;
    if(!calculate("sin(log(e^pi))", 978.77093770393308603e-3)) return false;

    done(); return true;
}
//------------------------------------------------------------------------------

void treeView(const char* expression)
{
    std::string tree;
    calc.buildTree(expression);
    calc.showTree (&tree);
    info("%s\n%s", expression, tree.c_str());
}
//------------------------------------------------------------------------------

bool testTreeView()
{
    start("Viewing the tree");

    treeView(" 3 + x  * 5");
    treeView("(3 + x) * 5");
    treeView("sin(log(e^(pi + x)))");
    treeView("d(sin(log(e^(pi + x))))dx");

    done(); return true;
}
//------------------------------------------------------------------------------

int main()
{
    setupTerminal();

    printf("\n\n");
    if(!testConstants  ()) goto MainError;
    if(!testExpressions()) goto MainError;
    if(!testTreeView   ()) goto MainError;

    info(ANSI_FG_GREEN "All OK"); done();
    return 0;

    MainError:
        done(); info(ANSI_FG_BRIGHT_RED "There were errors"); done();
        return -1;
}
//------------------------------------------------------------------------------

