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

#include "Calculator.h"
//------------------------------------------------------------------------------

using namespace std;
//------------------------------------------------------------------------------

#define e  2.718281828459045235360L
#define pi 3.141592653589793238463L
//------------------------------------------------------------------------------

void operator+= (string& S, double d)
{
    char s[0x100]; sprintf(s, "%0.12lg", d); S += s;
}
//------------------------------------------------------------------------------

void operator+= (string& S, int i)
{
    char s[0x100]; sprintf(s, "%d", i); S += s;
}
//------------------------------------------------------------------------------

void operator+= (string& S, void* p)
{
    char s[0x100]; sprintf(s, "%p", p); S += s;
}
//------------------------------------------------------------------------------

long double fix(long double x)
{
    if(x < 0.0) return ceill(x);
    else        return floorl(x);
}
//------------------------------------------------------------------------------

Calculator::Calculator()
{
    tree    = 0;
    measure = Radians;

    constants["e"        ] = e;
    constants["pi"       ] = pi;
    constants["\\c"      ] = 2.99792458e8L;
    constants["\\G"      ] = 6.67259e-11L;
    constants["\\g"      ] = 9.80665L;
    constants["\\me"     ] = 9.1093897e-31L;
    constants["\\mp"     ] = 1.6726231e-27L;
    constants["\\mn"     ] = 1.6749286e-27L;
    constants["\\u"      ] = 1.6605402e-27L;
    constants["\\e"      ] = 1.60217646e-19L;
    constants["\\h"      ] = 6.6260689633e-34L;
    constants["\\k"      ] = 1.380650424e-23L;
    constants["\\mu"     ] = pi*4e-7L;
    constants["\\epsilon"] = 8.854187817e-12L;
    constants["\\Phi"    ] = 2.06783461e-15L;
    constants["\\NA"     ] = 6.0221417930e23L;
    constants["\\R"      ] = 8.31447215L;
    constants["\\rhoAl"  ] = 26.53e-9L;
    constants["\\rhoCu"  ] = 16.78e-9L;
    constants["\\F"      ] = 9.6485304e4L;
    constants["\\AU"     ] = 1.4959787e11L;
    constants["\\pc"     ] = 3.0856776e16L;
    constants["\\mil"    ] = 25.4e-6L;
}
//------------------------------------------------------------------------------

Calculator::~Calculator()
{
    deleteTree(tree);
}
//------------------------------------------------------------------------------

Calculator::Node::Node()
{
    operation = Val;
    value     = 0.0;
    left      = 0;
    right     = 0;
    other     = 0;
}
//------------------------------------------------------------------------------

void Calculator::deleteTree(Node* root)
{
    if(root){
        deleteTree(root->left );
        deleteTree(root->right);
        deleteTree(root->other);
        delete root;
    }
}
//------------------------------------------------------------------------------

Calculator::Node* Calculator::copyNode(Node* root)
{
    Node* N = new Node;
    if(root){
        N->operation = root->operation;
        N->value     = root->value;
        N->left      = copyNode(root->left );
        N->right     = copyNode(root->right);
        N->other     = copyNode(root->other);
        N->name      =         (root->name );
        return N;
    }else{
        return 0;
    }
}
//------------------------------------------------------------------------------

Calculator::Node* Calculator::newNode(Node* root)
{
    Node* N = new Node;
    if(root){
        N->operation = root->operation;
        N->value     = root->value;
        N->left      = root->left;
        N->right     = root->right;
        N->other     = root->other;
        N->name      = root->name;
    }
    return N;
}
//------------------------------------------------------------------------------

bool Calculator::conditionOp(Node* root)
{
    if(buffer[index] == '['){
        index++;
        root->operation = Condition;
        root->left  = newNode();
        root->right = newNode();
        if(!logicOp(root->left)) return false;
        if(buffer[index++] != ']') return false;
        if(!logicOp(root->right)) return false;
        if(buffer[index] == 0){
            return true;
        }else{
            root->other = newNode();
            return conditionOp(root->other);
        }
    }else{
        return logicOp(root);
    }
}
//------------------------------------------------------------------------------

bool Calculator::logicOp(Node* root)
{
    Node* N;

    if(addOp(root)){
        while(buffer[index]){
            switch(buffer[index]){
                case '>':
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    if(buffer[index+1] == '='){
                        root->operation = GreaterEqual;
                        index += 2;
                    }else{
                        root->operation = Greater;
                        index += 1;
                    }
                    if(!addOp(root->right)) return false;
                    break;
                case '<':
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    if(buffer[index+1] == '='){
                        root->operation = LessEqual;
                        index += 2;
                    }else{
                        root->operation = Less;
                        index += 1;
                    }
                    if(!addOp(root->right)) return false;
                    break;
                case '~':
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    if(buffer[index+1] == '='){
                        root->operation = NotEqual;
                        index += 2;
                    }else{
                        return false;
                    }
                    if(!addOp(root->right)) return false;
                    break;
                case '=':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Equal;
                    if(!addOp(root->right)) return false;
                    break;
                case '&':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = And;
                    if(!addOp(root->right)) return false;
                    break;
                case '|':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Or;
                    if(!addOp(root->right)) return false;
                    break;
                case ':':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Xor;
                    if(!addOp(root->right)) return false;
                    break;
                case 'a':
                    if((buffer[index+1] == 'n') && (buffer[index+2] == 'd')){
                        index += 3;
                        N = newNode(root);
                        root->left  = N;
                        root->right = newNode();
                        root->operation = bAnd;
                        if(!addOp(root->right)) return false;
                    }else{
                        return true;
                    }
                    break;
                case 'o':
                    if(buffer[index+1] == 'r'){
                        index += 2;
                        N = newNode(root);
                        root->left  = N;
                        root->right = newNode();
                        root->operation = bOr;
                        if(!addOp(root->right)) return false;
                    }else{
                        return true;
                    }
                    break;
                case 'x':
                    if((buffer[index+1] == 'o') && (buffer[index+2] == 'r')){
                        index += 3;
                        N = newNode(root);
                        root->left  = N;
                        root->right = newNode();
                        root->operation = bXor;
                        if(!addOp(root->right)) return false;
                    }else{
                        return true;
                    }
                    break;
                default:
                    return true;
            }
        }
    }else{
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

bool Calculator::addOp(Node* root)
{
    Node* N;

    if(mulOp(root)){
        while(buffer[index]){
            switch(buffer[index]){
                case '+':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Add;
                    if(!mulOp(root->right)) return false;
                    break;
                case '-':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Subtract;
                    if(!mulOp(root->right)) return false;
                    break;
                default:
                    return true;
            }
        }
    }else{
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

bool Calculator::mulOp(Node* root)
{
    Node* N;

    if(powerOp(root)){
        while(buffer[index]){
            switch(buffer[index]){
                case '*':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Multiply;
                    if(!powerOp(root->right)) return false;
                    break;
                case '/':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Divide;
                    if(!powerOp(root->right)) return false;
                    break;
                case '%':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Remainder;
                    if(!powerOp(root->right)) return false;
                    break;
                case 'r':
                    if((buffer[index+1] == 'e') && (buffer[index+2] == 'm')){
                        index += 3;
                        N = newNode(root);
                        root->left  = N;
                        root->right = newNode();
                        root->operation = Remainder;
                        if(!powerOp(root->right)) return false;
                    }else{
                        return true;
                    }
                    break;
                default:
                    return true;
            }
        }
    }else{
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

bool Calculator::powerOp(Node* root)
{
    Node* N;

    if(function(root)){
        while(buffer[index]){
            switch(buffer[index]){
                case '^':
                    index++;
                    N = newNode(root);
                    root->left  = N;
                    root->right = newNode();
                    root->operation = Power;
                    if(!function(root->right)) return false;
                    break;
                default:
                    return true;
            }
        }
    }else{
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

void Calculator::funcName(string* name)
{
    *name = "";

    int i = index;
    while(
        ((buffer[i] >= 'a') && (buffer[i] <= 'z')) ||
        ((buffer[i] >= 'A') && (buffer[i] <= 'Z')) ||
        ( buffer[i] == '\\'                      )
    ){
        *name += buffer[i++];
    }
}
//------------------------------------------------------------------------------

bool Calculator::function(Node* root)
{
    Node*  N;
    string s;
    bool   Minus = false;

    if(buffer[index] == '-'){
        index++;
        Minus = true;
    }else if(buffer[index] == '~'){
        index++;
        root->operation = Not;
        root->right = newNode();
        return Factorial(root->right);
    }

    funcName(&s);

    if((buffer[index] == 'd') && (buffer[index+1] == '(')){
        index++;
        if(!logicOp(root)) return false;
        if(buffer[index++] != 'd') return false;
        N = newNode();
        if(!value(N)) return false;
        if(N->operation != Var) return false;
        diff(root, N->name.c_str());
        deleteTree(N);
    }else if(s == "log"){
        index += 3;
        root->operation = Log;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "lb"){
        index += 2;
        root->operation = Log2;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "ln"){
        index += 2;
        root->operation = Ln;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "abs"){
        index += 3;
        root->operation = Abs;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "round"){
        index += 5;
        root->operation = Round;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "fix"){
        index += 3;
        root->operation = Fix;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "floor"){
        index += 5;
        root->operation = Floor;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "ceil"){
        index += 4;
        root->operation = Ceil;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "rand"){
        index += 4;
        root->operation = Rand;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "sin"){
        index += 3;
        root->operation = Sin;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "asin"){
        index += 4;
        root->operation = ASin;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "cos"){
        index += 3;
        root->operation = Cos;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "acos"){
        index += 4;
        root->operation = ACos;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "tan"){
        index += 3;
        root->operation = Tan;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "atan"){
        index += 4;
        root->operation = ATan;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "sec"){
        index += 3;
        root->operation = Sec;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "asec"){
        index += 4;
        root->operation = ASec;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "cosec"){
        index += 5;
        root->operation = Cosec;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "acosec"){
        index += 6;
        root->operation = ACosec;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "cot"){
        index += 3;
        root->operation = Cot;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "acot"){
        index += 4;
        root->operation = ACot;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "sinh"){
        index += 4;
        root->operation = Sinh;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "asinh"){
        index += 5;
        root->operation = ASinh;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "cosh"){
        index += 4;
        root->operation = Cosh;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "acosh"){
        index += 5;
        root->operation = ACosh;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "tanh"){
        index += 4;
        root->operation = Tanh;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "atanh"){
        index += 5;
        root->operation = ATanh;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "sech"){
        index += 4;
        root->operation = Sech;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "asech"){
        index += 5;
        root->operation = ASech;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "cosech"){
        index += 6;
        root->operation = Cosech;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "acosech"){
        index += 7;
        root->operation = ACosech;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "coth"){
        index += 4;
        root->operation = Coth;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "acoth"){
        index += 5;
        root->operation = ACoth;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else if(s == "not"){
        index += 3;
        root->operation = bNot;
        root->right = newNode();
        if(!Factorial(root->right)) return false;
    }else{
        if(Minus) index--;
        return Factorial(root);
    }

    if(Minus){
    N = newNode(root);
    root->operation = Multiply;
    root->left = newNode();
    root->left->value = -1.;
    root->right = N;
    }

    return true;
}
//------------------------------------------------------------------------------

bool Calculator::Factorial(Node* root)
{
Node* N;
if(value(root))
{
    if(buffer[index] == '!'){
        index++;
        N = newNode(root);
        root->left      = 0;
        root->right     = N;
        root->operation = Fact;
    }
}else{
    return false;
}
return true;
}
//------------------------------------------------------------------------------

bool Calculator::Exponent(Node* root)
{
    int  s = 1;
    int  i;
    bool Binary;

    if     (buffer[index] == 'e' || buffer[index] == 'E') Binary = false;
    else if(buffer[index] == 'p' || buffer[index] == 'P') Binary = true;
    else return true; // It's not an exponent, but not an error either
    index++;

    s = 1;
    if(buffer[index] == '+'){
        index++;
    }else if(buffer[index] == '-'){
        index++;
        s = -1;
    }
    i = 0;
    while((buffer[index] >= '0') && (buffer[index] <= '9')){
        i = i * 10 + (buffer[index] - '0');
        index++;
    }
    i *= s;

    if(Binary) root->value *= pow( 2.0, i);
    else       root->value *= pow(10.0, i);

    return true;
}
//------------------------------------------------------------------------------

bool Calculator::value(Node* root)
{
    bool        Minus = false;
    string      s;
    Node*       N;
    long double f;

    if(buffer[index] == '-'){
        index++;
        Minus = true;
    }

    if((buffer[index] == '0') && (buffer[index+1] == 'x')){
        index += 2;
        f = 0;
        while(((buffer[index] >= '0') && (buffer[index] <= '9')) ||
              ((buffer[index] >= 'a') && (buffer[index] <= 'f')) ||
              ((buffer[index] >= 'A') && (buffer[index] <= 'F')) ){
            if(buffer[index] <= '9'){
                f = f * 16. + (buffer[index] - '0');
            }else if(buffer[index] <= 'F'){
                f = f * 16. + (buffer[index] - 'A' + 10.);
            }else{
                f = f * 16. + (buffer[index] - 'a' + 10.);
            }
            index++;
        }
        if(Minus) f *= -1.;
        root->operation = Val;
        root->value     = f;
        if(!Exponent(root)) return false;

    }else if((buffer[index] == '0') && (buffer[index+1] == 'b')){
        index += 2;
        f = 0;
        while(buffer[index] == '0' || buffer[index] == '1'){
            f = f * 2. + buffer[index] - '0';
            index++;
        }
        if(Minus) f *= -1.;
        root->operation = Val;
        root->value     = f;
        if(!Exponent(root)) return false;

    }else if(((buffer[index] >= '0') && (buffer[index] <= '9')) ||
             ((buffer[index] == '.') || (buffer[index] == ',')) ){
        if(!parseFloat(&f)) return false;
        if(Minus) f *= -1.;
        root->operation = Val;
        root->value     = f;
        if(!Exponent(root)) return false;

    }else if(buffer[index] == '('){
        index++;
        if(!logicOp(root)) return false;
        if(buffer[index++] != ')') return false;
        if(Minus){
            N = newNode(root);
            root->operation   = Multiply;
            root->left        = newNode();
            root->left->value = -1.;
            root->right       = N;
        }
    }else{
        funcName(&s);
        if(s.length()){
            auto Constant = constants.find(s);
            if(Constant != constants.end()){
                index += Constant->first.length();
                root->operation = Val;
                root->value = Constant->second;
                if(Minus) root->value *= -1.;

            }else{
                index += s.length();
                root->operation = Var;
                root->name      = s;
                if(Minus){
                    N = newNode(root);
                    root->operation   = Multiply;
                    root->left        = newNode();
                    root->left->value = -1.;
                    root->right       = N;
                }
            }
        }else{
            return false;
        }
    }
    return true;
}
//------------------------------------------------------------------------------

bool Calculator::parseFloat(long double* f)
{
    int s = 1;
    int i;
    long double temp;

    temp = 0.0;
    while((buffer[index] >= '0') && (buffer[index] <= '9')){
        temp = temp * 10. + (buffer[index] - '0');
        index++;
    }
    temp *= s;

    if((buffer[index] == '.') || (buffer[index] == ',')){
        index++;
        i = -1;
        while((buffer[index] >= '0') && (buffer[index] <= '9')){
            temp += (buffer[index] - '0') * pow(10., i--);
            index++;
        }
    }
    *f = temp;

    return true;
}
//------------------------------------------------------------------------------

void Calculator::buildTree(const char* formula)
{
    deleteTree(tree);
    tree = 0;
    int j, q;

    for(j = 0; formula[j]; j++);
    buffer = new char[j+1];

    q = 0;
    for(j = 0; formula[j]; j++){
        if(formula[j] != ' ' && formula[j] != '\t') buffer[q++] = formula[j];
    }
    buffer[q] = 0;

    if(q){
        index = 0;
        tree  = newNode();
        if(!conditionOp(tree)){
            deleteTree(tree);
            tree = 0;
        }
    }

    delete[] buffer;

    while(simplify(tree));
}
//------------------------------------------------------------------------------

bool comp(char* a, char* b)
{
    for(int j = 0; j < 10; j++){
        if(a[j] != b[j]) return false;
    }
    return true;
}
//------------------------------------------------------------------------------

long double Calculator::calcTree(Node* root, const char* variable,
long double value)
{
    long double A;
    long double B;
    long double nan = 0.0/0.0;
    long double inf = 1.0/0.0;
    int i1, i2;

    if(root){
        if(root->right){ // function
            switch(root->operation){
                case Fact:
                    A = calcTree(root->right, variable, value);
                    if(A < 0.) return nan;
                    i1 = floor(A);
                    if(A != i1) return nan;
                    A = 1.;
                    for(; i1 > 1; i1--){
                        A *= i1;
                    }
                    return A;
                case Power:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    i1 = round(B);
                    if(i1 == B){
                        return pow(A, i1);
                    }else{
                        if(A < 0.) return nan;
                        if(comp((char*)(&A), (char*)(&nan)) ||
                           comp((char*)(&B), (char*)(&nan)) ||
                           A ==  inf                        ||
                           B ==  inf                        ||
                           A == -inf                        ||
                           B == -inf                        ){
                            return nan;
                        }else{
                            return pow(A, B);
                        }
                    }
                case Multiply:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    return A * B;
                case Divide:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(B == 0.){
                        if(A < 0.){
                            return -1*inf;
                        }else if (A > 0.){
                            return inf;
                        }else{
                            return nan;
                        }
                    }else{
                        return A / B;
                    }
                case Remainder:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(B == 0.){
                        return nan;
                    }else{
                        i1 = round(A);
                        i2 = round(B);
                        if((i1 == A) && (i2 == B)){
                            return i1 % i2;
                        }else{
                            if(((A > 0.) && (B > 0.)) ||
                            ((A < 0.) && (B < 0.)) ){
                                while(A > B) A -= B;
                                return A;
                            }else{
                                while(A < B) A += B;
                                return A;
                            }
                        }
                    }
                case Add:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    return A + B;
                case Subtract:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    return A - B;
                case Log:
                    A = calcTree(root->right, variable, value);
                    return log10l(A);
                case Log2:
                    A = calcTree(root->right, variable, value);
                    return log2l(A);
                case Ln:
                    A = calcTree(root->right, variable, value);
                    return logl(A);
                case Abs:
                    A = calcTree(root->right, variable, value);
                    return fabsl(A);
                case Round:
                    A = calcTree(root->right, variable, value);
                    return round(A);
                case Fix:
                    A = calcTree(root->right, variable, value);
                    return fix(A);
                case Floor:
                    A = calcTree(root->right, variable, value);
                    return floorl(A);
                case Ceil:
                    A = calcTree(root->right, variable, value);
                    return ceill(A);
                case Rand:
                    A = calcTree(root->right, variable, value);
                    B = rand();
                    B /= RAND_MAX;
                    return A * B;
                case Sin:
                    A = calcTree(root->right, variable, value);
                    if(measure == Degrees) A *= pi/180.;
                    return sinl(A);
                case ASin:
                    A = calcTree(root->right, variable, value);
                    A = asinl(A);
                    if(measure == Degrees) A *= 180./pi;
                    return A;
                case Cos:
                    A = calcTree(root->right, variable, value);
                    if(measure == Degrees) A *= pi/180.;
                    return cosl(A);
                case ACos:
                    A = calcTree(root->right, variable, value);
                    A = acosl(A);
                    if(measure == Degrees) A *= 180./pi;
                    return A;
                case Tan:
                    A = calcTree(root->right, variable, value);
                    if(measure == Degrees) A *= pi/180.;
                    return tanl(A);
                case ATan:
                    A = calcTree(root->right, variable, value);
                    A = atanl(A);
                    if(measure == Degrees) A *= 180./pi;
                    return A;
                case Sec:
                    A = calcTree(root->right, variable, value);
                    if(measure == Degrees) A *= pi/180.;
                    A = cosl(A);
                    if(A == 0.){
                        return inf;
                    }else{
                        return 1/A;
                    }
                case ASec:
                    A = calcTree(root->right, variable, value);
                    if(A == 0.){
                        return nan;
                    }else{
                        A = 1/A;
                        A = acosl(A);
                        if(measure == Degrees) A *= 180./pi;
                        return A;
                    }
                case Cosec:
                    A = calcTree(root->right, variable, value);
                    if(measure == Degrees) A *= pi/180.;
                    A = sinl(A);
                    if(A == 0.){
                        return inf;
                    }else{
                        return 1/A;
                    }
                case ACosec:
                    A = calcTree(root->right, variable, value);
                    if(A == 0.){
                        return nan;
                    }else{
                        A = 1/A;
                        A = asinl(A);
                        if(measure == Degrees) A *= 180./pi;
                        return A;
                    }
                case Cot:
                    A = calcTree(root->right, variable, value);
                    if(measure == Degrees) A *= pi/180.;
                    A = tanl(A);
                    if(A == 0.){
                        return inf;
                    }else{
                        return 1/A;
                    }
                case ACot:
                    A = calcTree(root->right, variable, value);
                    if(A == 0.){
                        return nan;
                    }else{
                        A = 1/A;
                        A = atanl(A);
                        if(measure == Degrees) A *= 180./pi;
                        return A;
                    }
                case Sinh:
                    A = calcTree(root->right, variable, value);
                    return sinhl(A);
                case ASinh:
                    A = calcTree(root->right, variable, value);
                    return logl(A + sqrtl(A*A + 1));
                case Cosh:
                    A = calcTree(root->right, variable, value);
                    return coshl(A);
                case ACosh:
                    A = calcTree(root->right, variable, value);
                    if(A < 1.){
                        return nan;
                    }else{
                        return logl(A + sqrt(A*A - 1));
                    }
                case Tanh:
                    A = calcTree(root->right, variable, value);
                    return tanhl(A);
                case ATanh:
                    A = calcTree(root->right, variable, value);
                    if((-1. < A) && (A < 1.)){
                        return 0.5 * logl((1 + A)/(1 - A));
                    }else{
                        return nan;
                    }
                case Sech:
                    A = calcTree(root->right, variable, value);
                    A = coshl(A);
                    if(A == 0.){
                        return inf;
                    }else{
                        return 1/A;
                    }
                case ASech:
                    A = calcTree(root->right, variable, value);
                    if((A <= 0.) || (A > 1.)){
                        return nan;
                    }else{
                        return logl(1/A + sqrt(1/A/A - 1));
                    }
                case Cosech:
                    A = calcTree(root->right, variable, value);
                    A = sinh(A);
                    if(A == 0.){
                        return inf;
                    }else{
                        return 1/A;
                    }
                case ACosech:
                    A = calcTree(root->right, variable, value);
                    if(A == 0.){
                        return nan;
                    }else{
                        return logl(1/A + sqrt(1/A/A + 1));
                    }
                case Coth:
                    A = calcTree(root->right, variable, value);
                    A = tanhl(A);
                    if(A == 0.){
                        return inf;
                    }else{
                        return 1/A;
                    }
                case ACoth:
                    A = calcTree(root->right, variable, value);
                    if((-1 <= A) && (A <= 1.)){
                        return nan;
                    }else{
                        return 0.5 * logl((A+1)/(A-1));
                    }
                case Condition:
                    A = calcTree(root->left, variable, value);
                    if(A != 0.){
                        return calcTree(root->right, variable, value);
                    }else{
                        return calcTree(root->other, variable, value);
                    }
                case Greater:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(A > B){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case Less:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(A < B){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case Equal:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(A == B){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case GreaterEqual:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(A >= B){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case LessEqual:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(A <= B){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case NotEqual:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(A != B){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case Not:
                    A = calcTree(root->right, variable, value);
                    if(A == 0.){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case bNot:
                    A = calcTree(root->right, variable, value);
                    return ~((uint64_t)A);
                case And:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if((A != 0.) && (B != 0.)){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case Or:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if((A != 0.) || (B != 0.)){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case Xor:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    if(((A != 0.) && (B == 0.)) ||
                    ((A == 0.) && (B != 0.)) ){
                        return 1.;
                    }else{
                        return 0.;
                    }
                case bAnd:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    return ((uint64_t)A) & ((uint64_t)B);
                case bOr:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    return ((uint64_t)A) | ((uint64_t)B);
                case bXor:
                    A = calcTree(root->left , variable, value);
                    B = calcTree(root->right, variable, value);
                    return ((uint64_t)A) ^ ((uint64_t)B);
                default:
                    break;
            }
        }else{ // value
            if(root->operation == Var){
                if(root->name == variable){
                    return value;
                }else{
                    return 0.;
                }
            }else{
                return root->value;
            }
        }
    }else{
        return 0.;
    }
    return 0.;
}
//------------------------------------------------------------------------------

long double Calculator::calculateTree(const char* variable, long double value)
{
    return calcTree(tree, variable, value);
}
//------------------------------------------------------------------------------

long double Calculator::calculate(
    const char* formula,
    const char* variable,
    long double value
){
    buildTree(formula);
    return calculateTree(variable, value);
}
//------------------------------------------------------------------------------

bool Calculator::simplify(Node* root)
{
    Node* N;
    if(root){
        while(simplify(root->left ));
        while(simplify(root->right));

        if(root->right){ // function
            if(root->right->operation == Val  &&
                  root->operation        != Rand
            ){
                if(root->left            == 0  ||
                      root->left->operation == Val
                ){
                    root->value = calcTree(root, 0, 0.);
                    root->operation = Val;
                    deleteTree(root->left);
                    root->left = 0;
                    deleteTree(root->right);
                    root->right = 0;
                    return true;
                }
            }

            switch(root->operation){
                case Power:
                    if(root->right->operation == Val){
                        if(root->right->value == 0.){
                            deleteTree(root->left);
                            root->left = 0;
                            deleteTree(root->right);
                            root->right = 0;
                            root->operation = Val;
                            root->value = 1.;
                            return true;
                        }else if(root->right->value == 1.){
                            deleteTree(root->right);
                            N = root->left;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }
                    break;

                case Multiply:
                    if(root->right->operation == Val){
                        if(root->right->value == 0.){
                            deleteTree(root->left);
                            root->left = 0;
                            deleteTree(root->right);
                            root->right = 0;
                            root->operation = Val;
                            root->value = 0.;
                            return true;
                        }else if(root->right->value == 1.){
                            deleteTree(root->right);
                            N = root->left;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }else if(root->left->operation == Val){
                        if(root->left->value == 0.){
                            deleteTree(root->left);
                            root->left = 0;
                            deleteTree(root->right);
                            root->right = 0;
                            root->operation = Val;
                            root->value = 0.;
                            return true;
                        }else if(root->left->value == 1.){
                            deleteTree(root->left);
                            N = root->right;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }
                    break;

                case Divide:
                    if(root->right->operation == Val){
                        if(root->right->value == 1.){
                            deleteTree(root->right);
                            N = root->left;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }else{
                        if(root->left->operation == Val &&
                        root->left->value     == 0.  ){
                            root->operation = Val;
                            root->value = 0.;
                            deleteTree(root->left);
                            root->left = 0;
                            deleteTree(root->right);
                            root->right = 0;
                            return true;
                        }
                    }
                    break;

                case Add:
                    if(root->right->operation == Val){
                        if(root->right->value == 0.){
                            deleteTree(root->right);
                            N = root->left;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }else if(root->left->operation == Val){
                        if(root->left->value == 0.){
                            deleteTree(root->left);
                            N = root->right;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }
                    break;

                case Subtract:
                    if(root->right->operation == Val){
                        if(root->right->value == 0.){
                            deleteTree(root->right);
                            N = root->left;
                            root->operation = N->operation;
                            root->value     = N->value;
                            root->left      = N->left;
                            root->right     = N->right;
                            root->other     = N->other;
                            root->name      = N->name;
                            delete N;
                            return true;
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }
    return false;
}
//------------------------------------------------------------------------------

void Calculator::diff(Node* root, const char* variable)
{
    Node* N;
    Node* Temp;
    if(root){
        if(root->right){ // function
            switch(root->operation){
                case Power:
                    N = newNode(root);
                    root->operation = Add;
                    root->left      = newNode();
                    root->right     = newNode();

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = copyNode(N->left);
                    diff(Temp->right, variable);

                    Temp            = Temp->left;
                    Temp->operation = Multiply;
                    Temp->left      = copyNode(N->right);
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Power;
                    Temp->left      = copyNode(N->left);
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Subtract;
                    Temp->left      = copyNode(N->right);
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right;
                    Temp->operation = Multiply;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = copyNode(N->right);
                    diff(Temp->right, variable);

                    Temp            = Temp->left;
                    Temp->operation = Ln;
                    Temp->right     = copyNode(N->left);
                    break;
                case Multiply:
                    N = newNode(root);
                    root->operation = Add;
                    root->left      = N;
                    root->right     = copyNode(N);
                    diff(root->left ->left , variable);
                    diff(root->right->right, variable);
                    break;
                case Divide:
                    N = newNode(root);
                    root->operation = Divide;
                    root->left      = newNode();
                    root->right     = newNode();

                    Temp            = root->left;
                    Temp->operation = Subtract;
                    Temp->left      = N;
                    Temp->right     = copyNode(N);
                    diff(Temp->left ->left , variable);
                    diff(Temp->right->right, variable);

                    Temp            = root->right;
                    Temp->operation = Power;
                    Temp->left      = copyNode(N->right);
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case Add:
                    diff(root->left , variable);
                    diff(root->right, variable);
                    break;
                case Subtract:
                    diff(root->left , variable);
                    diff(root->right, variable);
                    break;
                case Log:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = N;

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 0.434294481903251828; // log(e)
                    break;
                case Log2:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = N;

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.44269504088896341; // log2(e)
                    break;
                case Ln:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = N;
                    root->right     = copyNode(N);
                    diff(root->left, variable);
                    break;
                case Sin:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Cos;
                    Temp->right     = N;
                    break;
                case ASin:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->left->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Cos:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Sin;
                    Temp->right     = N;
                    break;
                case ACos:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->left->right->left->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->left->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Tan:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Sec;
                    Temp->right     = N;

                    Temp            = root->left->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case ATan:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Add;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case Sec:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Sec;
                    Temp->right     = copyNode(N);

                    Temp            = root->left->right;
                    Temp->operation = Tan;
                    Temp->right     = N;
                    break;
                case ASec:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Abs;
                    Temp->right     = copyNode(N);

                    Temp            = root->right->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->right->right->left->right;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Cosec:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = newNode();

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Cosec;
                    Temp->right     = copyNode(N);

                    Temp            = root->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = copyNode(N);
                    diff(Temp->right, variable);

                    Temp            = Temp->left;
                    Temp->operation = Cot;
                    Temp->right     = N;
                    break;
                case ACosec:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Abs;
                    Temp->right     = copyNode(N);

                    Temp            = root->left->right->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->left->right->right->left->right;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->left->right->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Cot:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Cosec;
                    Temp->right     = N;

                    Temp            = root->left->right->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case ACot:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1;

                    Temp            = root->left->right;
                    Temp->operation = Add;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1;

                    Temp            = root->left->right->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case Sinh:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Cosh;
                    Temp->right     = N;
                    break;
                case ASinh:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Add;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->right->left->right;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Cosh:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Sinh;
                    Temp->right     = N;
                    break;
                case ACosh:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->right->left->right;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Tanh:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Sech;
                    Temp->right     = N;

                    Temp            = root->left->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case ATanh:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case Sech:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = newNode();

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Sech;
                    Temp->right     = copyNode(N);

                    Temp            = root->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = copyNode(N);
                    diff(Temp->right, variable);

                    Temp            = Temp->left;
                    Temp->operation = Tanh;
                    Temp->right     = N;
                    break;
                case ASech:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Multiply;
                    Temp->left      = copyNode(N);
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->left->right->right->left->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->left->right->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Cosech:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = newNode();

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Cosech;
                    Temp->right     = copyNode(N);

                    Temp            = root->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = copyNode(N);
                    diff(Temp->right, variable);

                    Temp            = Temp->left;
                    Temp->operation = Coth;
                    Temp->right     = N;
                    break;
                case ACosech:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Divide;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Abs;
                    Temp->right     = copyNode(N);

                    Temp            = root->left->right->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Add;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->left->right->right->left->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;

                    Temp            = root->left->right->right->right;
                    Temp->operation = Val;
                    Temp->value     = 0.5;
                    break;
                case Coth:
                    N = root->right;
                    root->operation = Multiply;
                    root->left      = newNode();
                    root->right     = copyNode(N);
                    diff(root->right, variable);

                    Temp            = root->left;
                    Temp->operation = Multiply;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = -1.;

                    Temp            = root->left->right;
                    Temp->operation = Power;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Cosech;
                    Temp->right     = N;

                    Temp            = root->left->right->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                case ACoth:
                    N = root->right;
                    root->operation = Divide;
                    root->left      = copyNode(N);
                    root->right     = newNode();
                    diff(root->left, variable);

                    Temp            = root->right;
                    Temp->operation = Subtract;
                    Temp->left      = newNode();
                    Temp->right     = newNode();

                    Temp            = Temp->left;
                    Temp->operation = Val;
                    Temp->value     = 1.;

                    Temp            = root->right->right;
                    Temp->operation = Power;
                    Temp->left      = N;
                    Temp->right     = newNode();

                    Temp            = Temp->right;
                    Temp->operation = Val;
                    Temp->value     = 2.;
                    break;
                default:
                    deleteTree(root->right);
                    root->right = 0;
                    root->operation = Val;
                    root->value = 0.;
                    break;
            }
            while(simplify(root));
        }else{ // value
            if(root->operation == Var){
                if(root->name == variable){
                    root->operation = Val;
                    root->value = 1.;
                }
            }else{
                root->operation = Val;
                root->value = 0.;
            }
        }
    }
}
//------------------------------------------------------------------------------

void Calculator::viewTree(Node* root, string* result)
{
    string A, B;

    if(root){
        if(root->right){ // function
            switch(root->operation){
                case Fact:
                    viewTree(root->right, &A);
                    *result += A ;
                    *result += "!";
                    return;

                case Power:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result += "(";
                    *result += A ;
                    *result += ")^(";
                    *result += B ;
                    *result += ")";
                    return;

                case Multiply:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result += "(";
                    *result += A ;
                    *result += "*";
                    *result += B ;
                    *result += ")";
                    return;

                case Divide:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result += "(";
                    *result += A ;
                    *result += "/";
                    *result += B ;
                    *result += ")";
                    return;

                case Remainder:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result += "(";
                    *result += A ;
                    *result += "rem";
                    *result += B ;
                    *result += ")";
                    return;

                case Add:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result += "(";
                    *result += A ;
                    *result += "+";
                    *result += B ;
                    *result += ")";
                    return;

                case Subtract:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result += "(";
                    *result += A ;
                    *result += "-";
                    *result += B ;
                    *result += ")";
                    return;

                case Log:
                    viewTree(root->right, &A);
                    *result += "log(";
                    *result += A ;
                    *result += ")";
                    return;

                case Log2:
                    viewTree(root->right, &A);
                    *result += "log2(";
                    *result += A ;
                    *result += ")";
                    return;

                case Ln:
                    viewTree(root->right, &A);
                    *result += "ln(";
                    *result += A ;
                    *result += ")";
                    return;

                case Abs:
                    viewTree(root->right, &A);
                    *result += "abs(";
                    *result += A ;
                    *result += ")";
                    return;

                case Round:
                    viewTree(root->right, &A);
                    *result += "round(";
                    *result += A ;
                    *result += ")";
                    return;

                case Fix:
                    viewTree(root->right, &A);
                    *result += "fix(";
                    *result += A ;
                    *result += ")";
                    return;

                case Floor:
                    viewTree(root->right, &A);
                    *result += "floor(";
                    *result += A ;
                    *result += ")";
                    return;

                case Ceil:
                    viewTree(root->right, &A);
                    *result += "ceil(";
                    *result += A ;
                    *result += ")";
                    return;

                case Rand:
                    viewTree(root->right, &A);
                    *result += "rand(";
                    *result += A ;
                    *result += ")";
                    return;

                case Sin:
                    viewTree(root->right, &A);
                    *result += "sin(";
                    *result += A ;
                    *result += ")";
                    return;

                case ASin:
                    viewTree(root->right, &A);
                    *result += "asin(";
                    *result += A ;
                    *result += ")";
                    return;

                case Cos:
                    viewTree(root->right, &A);
                    *result += "cos(";
                    *result += A ;
                    *result += ")";
                    return;

                case ACos:
                    viewTree(root->right, &A);
                    *result += "acos(";
                    *result += A ;
                    *result += ")";
                    return;

                case Tan:
                    viewTree(root->right, &A);
                    *result += "tan(";
                    *result += A ;
                    *result += ")";
                    return;

                case ATan:
                    viewTree(root->right, &A);
                    *result += "atan(";
                    *result += A ;
                    *result += ")";
                    return;

                case Sec:
                    viewTree(root->right, &A);
                    *result += "sec(";
                    *result += A ;
                    *result += ")";
                    return;

                case ASec:
                    viewTree(root->right, &A);
                    *result += "asec(";
                    *result += A ;
                    *result += ")";
                    return;

                case Cosec:
                    viewTree(root->right, &A);
                    *result += "cosec(";
                    *result += A ;
                    *result += ")";
                    return;

                case ACosec:
                    viewTree(root->right, &A);
                    *result += "acosec(";
                    *result += A ;
                    *result += ")";
                    return;

                case Cot:
                    viewTree(root->right, &A);
                    *result += "cot(";
                    *result += A ;
                    *result += ")";
                    return;

                case ACot:
                    viewTree(root->right, &A);
                    *result += "acot(";
                    *result += A ;
                    *result += ")";
                    return;

                case Sinh:
                    viewTree(root->right, &A);
                    *result += "sinh(";
                    *result += A ;
                    *result += ")";
                    return;

                case ASinh:
                    viewTree(root->right, &A);
                    *result += "asinh(";
                    *result += A ;
                    *result += ")";
                    return;

                case Cosh:
                    viewTree(root->right, &A);
                    *result += "cosh(";
                    *result += A ;
                    *result += ")";
                    return;

                case ACosh:
                    viewTree(root->right, &A);
                    *result += "acosh(";
                    *result += A ;
                    *result += ")";
                    return;

                case Tanh:
                    viewTree(root->right, &A);
                    *result += "tanh(";
                    *result += A ;
                    *result += ")";
                    return;

                case ATanh:
                    viewTree(root->right, &A);
                    *result += "atanh(";
                    *result += A ;
                    *result += ")";
                    return;

                case Sech:
                    viewTree(root->right, &A);
                    *result += "sech(";
                    *result += A ;
                    *result += ")";
                    return;

                case ASech:
                    viewTree(root->right, &A);
                    *result += "asech(";
                    *result += A ;
                    *result += ")";
                    return;

                case Cosech:
                    viewTree(root->right, &A);
                    *result += "cosech(";
                    *result += A ;
                    *result += ")";
                    return;

                case ACosech:
                    viewTree(root->right, &A);
                    *result += "acosech(";
                    *result += A ;
                    *result += ")";
                    return;

                case Coth:
                    viewTree(root->right, &A);
                    *result += "coth(";
                    *result += A ;
                    *result += ")";
                    return;

                case ACoth:
                    viewTree(root->right, &A);
                    *result += "acoth(";
                    *result += A ;
                    *result += ")";
                    return;

                case Condition:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "[";
                    *result += A ;
                    *result += "]";
                    *result += B ;
                    viewTree(root->other, &A);
                    *result += A ;
                    return;

                case Greater:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += ">";
                    *result += B ;
                    *result += ")";
                    return;

                case Less:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += "<";
                    *result += B ;
                    *result += ")";
                    return;

                case Equal:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += "=";
                    *result += B ;
                    *result += ")";
                    return;

                case GreaterEqual:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += ">=";
                    *result += B ;
                    *result += ")";
                    return;

                case LessEqual:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += "<=";
                    *result += B ;
                    *result += ")";
                    return;

                case NotEqual:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += "~=";
                    *result += B ;
                    *result += ")";
                    return;

                case Not:
                    viewTree(root->left , &A);
                    *result  = "(~";
                    *result += A ;
                    *result += ")";
                    return;

                case bNot:
                    viewTree(root->left , &A);
                    *result  = "(not";
                    *result += A ;
                    *result += ")";
                    return;

                case And:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += "&";
                    *result += B ;
                    *result += ")";
                    return;

                case Or:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += "|";
                    *result += B ;
                    *result += ")";
                    return;

                case Xor:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += ":";
                    *result += B ;
                    *result += ")";
                    return;

                case bAnd:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += " and ";
                    *result += B ;
                    *result += ")";
                    return;

                case bOr:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += " or ";
                    *result += B ;
                    *result += ")";
                    return;

                case bXor:
                    viewTree(root->left , &A);
                    viewTree(root->right, &B);
                    *result  = "(";
                    *result += A ;
                    *result += " xor ";
                    *result += B ;
                    *result += ")";
                    return;

                default:
                    *result  = "(Unknown operation: ";
                    *result += (int)root->operation;
                    *result += ", value: 0x";
                    *result += (double)round(root->value);
                    *result += ", name: ";
                    *result += root->name;
                    *result += ", left: ";
                    *result += root->left;
                    *result += ", right: ";
                    *result += root->right;
                    *result += ", other: ";
                    *result += root->other;
                    *result += ")";
                    return;
            }
        }else{ // value
            if(root->operation == Var){
                *result = root->name;
                return;

            }else if(root->operation == Val){
                if(root->value < 0.0){
                    *result  = '(';
                    *result += (double)root->value;
                    *result += ')';
                }else{
                    *result  = "";
                    *result += (double)root->value;
                }
                return;

            }else{
                *result  = "(Invalid node: operation: ";
                *result += (int)root->operation;
                *result += ", value: ";
                *result += (double)root->value;
                *result += ", name: ";
                *result += root->name;
                *result += ", left: ";
                *result += root->left;
                *result += ", right: ";
                *result += root->right;
                *result += ", other: ";
                *result += root->other;
                *result += ")";
                return;
            }
        }
    }else{
        *result = "";
    }
    *result = "";
}
//------------------------------------------------------------------------------

void Calculator::showTree(string* result)
{
    *result = "";
    viewTree(tree, result);
}
//------------------------------------------------------------------------------

