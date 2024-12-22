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

#ifndef Calculator_h
#define Calculator_h
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <map>
#include <string>
//------------------------------------------------------------------------------

class Calculator{
    public:
        enum Measure{
            Degrees = 1,
            Radians = 0
        };
    //--------------------------------------------------------------------------

    private:
        std::map<std::string, long double> constants;

        enum Operation{
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

        struct Node{
            Operation   operation;
            long double value;
            std::string name;
            Node*       left;
            Node*       right;
            Node*       other;

            Node();
        };

        Node* tree;
        char* buffer;
        int   index;

        Node* newNode (Node* root = 0);
        Node* copyNode(Node* root);

        void funcName(std::string* name);

        bool conditionOp(Node* root);
        bool logicOp    (Node* root);
        bool addOp      (Node* root);
        bool mulOp      (Node* root);
        bool powerOp    (Node* root);
        bool function   (Node* root);
        bool Factorial  (Node* root);
        bool Exponent   (Node* root);
        bool value      (Node* root);
        bool parseFloat (long double* f);

        void        viewTree  (Node* root, std::string* result);
        long double calcTree  (Node* root, const char* variable, long double value);
        void        deleteTree(Node* root);
        bool        simplify  (Node* root);
        void        diff      (Node* root, const char* variable);
    //--------------------------------------------------------------------------

    public:
        Measure measure;
        Calculator();
       ~Calculator();

        void        buildTree    (const char * formula);
        long double calculateTree(const char * variable = "", long double value = 0.);
        void        showTree     (std::string* result);
        long double calculate    (const char * formula, const char* variable = "",
                                  long double  value = 0.);
};
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
