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

#ifndef Calculator_h
#define Calculator_h
//------------------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
//------------------------------------------------------------------------------

#include "LLRBTree.h"
#include "UnicodeString.h"
//------------------------------------------------------------------------------

class Calculator{
  public:
    enum MEASURE{
      Degrees = 1,
      Radians = 0
    };
//------------------------------------------------------------------------------

  private:
    enum OPERATION{
      Fact     , Power   , Multiply, Divide , Remainder   ,
      Add      , Subtract, Log     , Log2   , Ln          ,
      Abs      , Round   , Fix     , Floor  , Ceil        , Rand     ,
      Sin      , ASin    , Cos     , ACos   , Tan         , ATan     ,
      Sec      , ASec    , Cosec   , ACosec , Cot         , ACot     ,
      Sinh     , ASinh   , Cosh    , ACosh  , Tanh        , ATanh    ,
      Sech     , ASech   , Cosech  , ACosech, Coth        , ACoth    ,
      Condition, Greater , Less    , Equal  , GreaterEqual, LessEqual,
      NotEqual , Not     ,
      And      , Or      , Xor     ,
      bAnd     , bOr     , bXor    , bNot   ,
      Var      , Val
    };

    struct NODE{
      OPERATION   Operation;
      long double Value;
      UnicodeString      Name;
      NODE*       Left;
      NODE*       Right;
      NODE*       Other;

      NODE();
    };

    NODE* Tree;
    char* Buffer;
    int   Index;

    NODE* NewNode (NODE* Root = 0);
    NODE* CopyNode(NODE* Root);

    void FuncName(UnicodeString* Name);

    bool ConditionOp(NODE* Root);
    bool LogicOp    (NODE* Root);
    bool AddOp      (NODE* Root);
    bool MulOp      (NODE* Root);
    bool PowerOp    (NODE* Root);
    bool Function   (NODE* Root);
    bool Factorial  (NODE* Root);
    bool Exponent   (NODE* Root);
    bool Value      (NODE* Root);
    bool Float      (long double* f);

    void        ViewTree  (NODE* Root, UnicodeString* Result, unsigned BufferSize);
    long double CalcTree  (NODE* Root, const char* Variable, long double Value);
    void        DeleteTree(NODE* Root);
    bool        Simplify  (NODE* Root);
    void        Diff      (NODE* Root, const char* Variable);
//------------------------------------------------------------------------------

  public:
    MEASURE Measure;
    Calculator();
   ~Calculator();

    void        BuildTree    (const char* Formula);
    long double CalculateTree(const char* Variable = "", long double Value = 0.);
    void        ShowTree     (UnicodeString* Result, unsigned BufferSize);
    long double Calculate    (const char* Formula, const char* Variable = "",
                              long double Value = 0.);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
