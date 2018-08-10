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

#include "UnicodeString.h"
//------------------------------------------------------------------------------

UNICODE_STRING::UNICODE_STRING(){
  Valid_8     = Valid_16     = false;
  Length_8    = Length_16    = Length_32    = 0;
  Allocated_8 = Allocated_16 = Allocated_32 = 4;

  Data_8  = new char  [Allocated_8 ]; Data_8 [0] = 0;
  Data_16 = new char16[Allocated_16]; Data_16[0] = 0;
  Data_32 = new char32[Allocated_32]; Data_32[0] = 0;
}
//------------------------------------------------------------------------------

UNICODE_STRING::~UNICODE_STRING(){
  delete[] Data_8;
  delete[] Data_16;
  delete[] Data_32;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Invalidate(){
  if(Valid_8){
    delete[] Data_8;
    Length_8    = 0;
    Allocated_8 = 4;
    Data_8      = new char[Allocated_8];
    Data_8[0]   = 0;
  }

  if(Valid_16){
    delete[] Data_16;
    Length_16    = 0;
    Allocated_16 = 4;
    Data_16      = new char16[Allocated_16];
    Data_16[0]   = 0;
  }

  Valid_8 = Valid_16 = false;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Append(char Data){
  size_t j;
  char*  Temp;

  if(Length_8+1 == Allocated_8){
    Allocated_8 <<= 1;
    Temp = new char[Allocated_8];

    for(j = 0; j <= Length_8; j++) Temp[j] = Data_8[j];

    delete[] Data_8;
    Data_8 = Temp;
  }

  Data_8[Length_8++] = Data;
  Data_8[Length_8  ] = 0;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Append(char16 Data){
  size_t  j;
  char16* Temp;

  if(Length_16+1 == Allocated_16){
    Allocated_16 <<= 1;
    Temp = new char16[Allocated_16];

    for(j = 0; j <= Length_16; j++) Temp[j] = Data_16[j];

    delete[] Data_16;
    Data_16 = Temp;
  }

  Data_16[Length_16++] = Data;
  Data_16[Length_16  ] = 0;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Append(char32 Data){
  size_t  j;
  char32* Temp;

  if(Length_32+1 == Allocated_32){
    Allocated_32 <<= 1;
    Temp = new char32[Allocated_32];

    for(j = 0; j <= Length_32; j++) Temp[j] = Data_32[j];

    delete[] Data_32;
    Data_32 = Temp;
  }

  Data_32[Length_32++] = Data;
  Data_32[Length_32  ] = 0;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Update8(){
  if(Valid_8) return;

  Length_8  = 0;
  Data_8[0] = 0;

  size_t j;

  for(j = 0; Data_32[j]; j++){
    if(Data_32[j] >> 31){ // Invalid code-point for UTF-8, so do nothing

    }else if(Data_32[j] >> 26){ // 6-Byte
      Append((char)(0xFC | ((Data_32[j] >> 30)       )));
      Append((char)(0x80 | ((Data_32[j] >> 24) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j] >> 18) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j] >> 12) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j] >>  6) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j]      ) & 0x3F)));

    }else if(Data_32[j] >> 21){ // 5-Byte
      Append((char)(0xF8 | ((Data_32[j] >> 24)       )));
      Append((char)(0x80 | ((Data_32[j] >> 18) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j] >> 12) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j] >>  6) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j]      ) & 0x3F)));

    }else if(Data_32[j] >> 16){ // 4-Byte
      Append((char)(0xF0 | ((Data_32[j] >> 18)       )));
      Append((char)(0x80 | ((Data_32[j] >> 12) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j] >>  6) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j]      ) & 0x3F)));

    }else if(Data_32[j] >> 11){ // 3-Byte
      Append((char)(0xE0 | ((Data_32[j] >> 12)       )));
      Append((char)(0x80 | ((Data_32[j] >>  6) & 0x3F)));
      Append((char)(0x80 | ((Data_32[j]      ) & 0x3F)));

    }else if(Data_32[j] >>  7){ // 2-Byte
      Append((char)(0xC0 | ((Data_32[j] >>  6)       )));
      Append((char)(0x80 | ((Data_32[j]      ) & 0x3F)));

    }else{ // 1-Byte
      Append((char)Data_32[j]);
    }
  }
  Valid_8 = true;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Update16(){
  if(Valid_16) return;

  Length_16  = 0;
  Data_16[0] = 0;

  size_t j;

  for(j = 0; Data_32[j]; j++){
    if(Data_32[j] >> 16){ // Encoded
      Append((char16)(((Data_32[j] & 0x000FFFFF) >> 10) | 0xD800));
      Append((char16)(((Data_32[j] & 0x000003FF)      ) | 0xDC00));

    }else{ // Direct
      Append((char16)Data_32[j]);
    }
  }

  Valid_16 = true;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Update32_From8(){
  Valid_8  = true;
  Valid_16 = false;

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  size_t j;
  char32 Temp;

  j = 0;
  while(Data_8[j]){
    if(Data_8[j] & 0x80){
      if(Data_8[j] & 0x40){
        if(Data_8[j] & 0x20){
          if(Data_8[j] & 0x10){
            if(Data_8[j] & 0x08){
              if(Data_8[j] & 0x04){
                if(Data_8[j] & 0x02){ // Invalid byte
                  Temp = Data_8[j++];

                }else{ // 6 bytes
                  Temp = Data_8[j++] & 0x01;
                  if((Data_8[j] & 0xC0) == 0x80)
                    Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
                  if((Data_8[j] & 0xC0) == 0x80)
                    Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
                  if((Data_8[j] & 0xC0) == 0x80)
                    Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
                  if((Data_8[j] & 0xC0) == 0x80)
                    Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
                  if((Data_8[j] & 0xC0) == 0x80)
                    Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
                }
              }else{ // 5 bytes
              Temp = Data_8[j++] & 0x03;
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
              }
            }else{ // 4 bytes
              Temp = Data_8[j++] & 0x07;
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
              if((Data_8[j] & 0xC0) == 0x80)
                Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
            }
          }else{ // 3 bytes
          Temp = Data_8[j++] & 0x0F;
          if((Data_8[j] & 0xC0) == 0x80)
            Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
          if((Data_8[j] & 0xC0) == 0x80)
            Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
          }
        }else{ // 2 bytes
          Temp = Data_8[j++] & 0x1F;
          if((Data_8[j] & 0xC0) == 0x80)
            Temp = (Temp << 6) | (Data_8[j++] & 0x3F);
        }
      }else{ // Unsynchronised continuation byte
        Temp = Data_8[j++];
      }
    }else{ // 1 byte
      Temp = Data_8[j++];
    }
    Append(Temp);
  }
}
//------------------------------------------------------------------------------

void UNICODE_STRING::Update32_From16(){
  Valid_8  = false;
  Valid_16 = true;

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  size_t j;
  char32 Temp;

  j = 0;
  while(Data_16[j]){
    if(
      ((Data_16[j  ] & 0xFC00) == 0xD800) &&
      ((Data_16[j+1] & 0xFC00) == 0xDC00)
    ){ // 2 words
      Temp =                 Data_16[j++] & 0x03FF;
      Temp = (Temp << 10) | (Data_16[j++] & 0x03FF);
    }else{ // 1 word
      Temp = Data_16[j++];
    }
    Append(Temp);
  }
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (char c){
  Invalidate();

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  Append((char32)((unsigned char)c));
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (char16 c){
  Invalidate();

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  Append((char32)c);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (char32 c){
  Invalidate();

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  Append(c);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (int d){
  size_t j, l;
  char32 c;

  Invalidate();

  bool Negative = false;

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  if(d){
    if(d < 0){
      d *= -1;
      Negative = true;
    }

    while(d){ // To append is faster that to prefix
      Append((char32)((d % 10) + '0'));
      d /= 10;
    }

    if(Negative) Append((char32)'-');

    // Reverse
    l = Length_32 / 2;
    for(j = 0; j < l; j++){
      c = Data_32[j];
      Data_32[j] = Data_32[Length_32-j-1];
      Data_32[Length_32-j-1] = c;
    }

  }else{
    Append((char32)'0');
  }
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (int64_t d){
  if(d < 0){
    operator+= ('-');
    operator+= ((uint64_t)(-d));
  }else{
    operator+= ((uint64_t)( d));
  }
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (uint64_t d){
  size_t j, l;
  char32 c;

  Invalidate();

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  if(d){
    while(d){ // To append is faster that to prefix
      Append((char32)((d % 10) + '0'));
      d /= 10;
    }

    // Reverse
    l = Length_32 / 2;
    for(j = 0; j < l; j++){
      c = Data_32[j];
      Data_32[j] = Data_32[Length_32-j-1];
      Data_32[Length_32-j-1] = c;
    }

  }else{
    Append((char32)'0');
  }
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (double f){
  SetFloat(f, 9, false);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (const char* String){
  size_t j;
  Invalidate();

  delete[] Data_8;
  Length_8    = 0;
  Allocated_8 = 4;
  Data_8      = new char[Allocated_8]; Data_8[0] = 0;

  for(j = 0; String[j]; j++) Append(String[j]);

  Update32_From8();
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (const char16* String){
  size_t j;
  Invalidate();

  delete[] Data_16;
  Length_16    = 0;
  Allocated_16 = 4;
  Data_16      = new char16[Allocated_16]; Data_16[0] = 0;

  for(j = 0; String[j]; j++) Append(String[j]);

  Update32_From16();
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (const char32* String){
  size_t j;
  Invalidate();

  delete[] Data_32;
  Length_32    = 0;
  Allocated_32 = 4;
  Data_32      = new char32[Allocated_32]; Data_32[0] = 0;

  for(j = 0; String[j]; j++) Append(String[j]);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator= (const UNICODE_STRING& String){
  size_t j;
  Invalidate();

  delete[] Data_32;
  Length_32    = String.Length_32;
  Allocated_32 = Length_32+1;
  Data_32      = new char32[Allocated_32];

  for(j = 0; String.Data_32[j]; j++) Data_32[j] = String.Data_32[j];
  Data_32[j] = 0;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (char c){
  operator+= ((char32)((unsigned char)c));
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (char16 c){
  operator+= ((char32)c);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (char32 c){
  Invalidate();

  Append(c);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (int d){
  UNICODE_STRING s;
  s = d;
  operator+= (s);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (int64_t d){
  UNICODE_STRING s;
  s = d;
  operator+= (s);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (uint64_t d){
  UNICODE_STRING s;
  s = d;
  operator+= (s);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (double f){
  UNICODE_STRING s;
  s = f;
  operator+= (s);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (const char* String){
  UNICODE_STRING Temp;
  Temp = String;
  operator+=(Temp);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (const char16* String){
  UNICODE_STRING Temp;
  Temp = String;
  operator+=(Temp);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (const char32* String){
  size_t j;
  Invalidate();

  for(j = 0; String[j]; j++) Append(String[j]);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::operator+= (const UNICODE_STRING& String){
  operator+= (String.Data_32);
}
//------------------------------------------------------------------------------

char32 UNICODE_STRING::operator[] (size_t Index){
  if(Index >= 0 && Index < Length_32) return Data_32[Index];
  return 0;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::SetLength32(size_t Length){
  Invalidate();

  if(Length < Length_32){
    Length_32          = Length;
    Data_32[Length_32] = 0;
  }else{
    while(Length_32 < Length) Append((char32)0);
  }
}
//------------------------------------------------------------------------------

size_t UNICODE_STRING::Length8(){
  Update8();
  return Length_8;
}
//------------------------------------------------------------------------------

size_t UNICODE_STRING::Length16(){
  Update16();
  return Length_16;
}
//------------------------------------------------------------------------------

size_t UNICODE_STRING::Length32(){
  return Length_32;
}
//------------------------------------------------------------------------------

char* UNICODE_STRING::UTF8(){
  Update8();
  return Data_8;
}
//------------------------------------------------------------------------------

char16* UNICODE_STRING::UTF16(){
  Update16();
  return Data_16;
}
//------------------------------------------------------------------------------

char32* UNICODE_STRING::UTF32(){
  return Data_32;
}
//------------------------------------------------------------------------------

char* UNICODE_STRING::New8(){
  Update8();

  size_t j;
  char*  Temp = new char[Length_8+1];

  for(j = 0; Data_8[j]; j++) Temp[j] = Data_8[j];
  Temp[j] = 0;

  return Temp;
}
//------------------------------------------------------------------------------

char16* UNICODE_STRING::New16(){
  Update16();

  size_t  j;
  char16* Temp = new char16[Length_16+1];

  for(j = 0; Data_16[j]; j++) Temp[j] = Data_16[j];
  Temp[j] = 0;

  return Temp;
}
//------------------------------------------------------------------------------

char32* UNICODE_STRING::New32(){
  size_t  j;
  char32* Temp = new char32[Length_32+1];

  for(j = 0; Data_32[j]; j++) Temp[j] = Data_32[j];
  Temp[j] = 0;

  return Temp;
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator== (const char* String){
  UNICODE_STRING s2;
  s2 = String;

  return operator== (s2.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator== (const char16* String){
  UNICODE_STRING s2;
  s2 = String;

  return operator== (s2.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator== (const char32* String){
  size_t j;

  for(j = 0; Data_32[j] && String[j]; j++){
    if(Data_32[j] != String[j]) return false;
  }
  if(Data_32[j] != String[j]) return false;
  return true;
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator== (UNICODE_STRING& String){
  return operator== (String.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator!= (const char* String){
  return !operator== (String);
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator!= (const char16* String){
  return !operator== (String);
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator!= (const char32* String){
  return !operator== (String);
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator!= (UNICODE_STRING& String){
  return !operator== (String);
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator< (const char* String){
  UNICODE_STRING s2;
  s2 = String;

  return operator< (s2.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator< (const char16* String){
  UNICODE_STRING s2;
  s2 = String;

  return operator< (s2.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator< (const char32* String){
  size_t j;

  for(j = 0; Data_32[j] && String[j]; j++){
    if(Data_32[j] < String[j]) return true;
    if(Data_32[j] > String[j]) return false;
  }
  if(Data_32[j] < String[j]) return true;
  if(Data_32[j] > String[j]) return false;
  return false;
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator< (UNICODE_STRING& String){
  return operator< (String.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator> (const char* String){
  UNICODE_STRING s2;
  s2 = String;

  return operator> (s2.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator> (const char16* String){
  UNICODE_STRING s2;
  s2 = String;

  return operator> (s2.UTF32());
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator> (const char32* String){
  size_t j;

  for(j = 0; Data_32[j] && String[j]; j++){
    if(Data_32[j] < String[j]) return false;
    if(Data_32[j] > String[j]) return true;
  }
  if(Data_32[j] < String[j]) return false;
  if(Data_32[j] > String[j]) return true;
  return false;
}
//------------------------------------------------------------------------------

bool UNICODE_STRING::operator> (UNICODE_STRING& String){
  return operator> (String.UTF32());
}
//------------------------------------------------------------------------------

int UNICODE_STRING::Compare(const UNICODE_STRING& String){
  size_t j;

  for(j = 0; Data_32[j] && String.Data_32[j]; j++){
    if(Data_32[j] < String.Data_32[j]) return -1;
    if(Data_32[j] > String.Data_32[j]) return  1;
  }
  if(Data_32[j] < String.Data_32[j]) return -1;
  if(Data_32[j] > String.Data_32[j]) return  1;

  return 0;
}
//------------------------------------------------------------------------------

int UNICODE_STRING::CompareNoCase(const UNICODE_STRING& String){
  UNICODE_STRING Left, Right;

  Left  = *this;
  Right = String;

  Left .UpperCase();
  Right.UpperCase();

  return Left.Compare(Right);
}
//------------------------------------------------------------------------------

int UNICODE_STRING::CompareNoCase(const char* String){
  UNICODE_STRING Left, Right;

  Left  = *this;
  Right = String;

  Left .UpperCase();
  Right.UpperCase();

  return Left.Compare(Right);
}
//------------------------------------------------------------------------------

bool operator== (const char* String1, UNICODE_STRING& String2){
  return String2 == String1;
}
//------------------------------------------------------------------------------

bool operator== (const char16* String1, UNICODE_STRING& String2){
  return String2 == String1;
}
//------------------------------------------------------------------------------

bool operator== (const char32* String1, UNICODE_STRING& String2){
  return String2 == String1;
}
//------------------------------------------------------------------------------

bool operator< (const char* String1, UNICODE_STRING& String2){
  return String2 > String1;
}
//------------------------------------------------------------------------------

bool operator< (const char16* String1, UNICODE_STRING& String2){
  return String2 > String1;
}
//------------------------------------------------------------------------------

bool operator< (const char32* String1, UNICODE_STRING& String2){
  return String2 > String1;
}
//------------------------------------------------------------------------------

bool operator> (const char* String1, UNICODE_STRING& String2){
  return String2 < String1;
}
//------------------------------------------------------------------------------

bool operator> (const char16* String1, UNICODE_STRING& String2){
  return String2 < String1;
}
//------------------------------------------------------------------------------

bool operator> (const char32* String1, UNICODE_STRING& String2){
  return String2 < String1;
}
//------------------------------------------------------------------------------

void UNICODE_STRING::SetHex(unsigned i, int Places){ // Hexadecimal
  Invalidate();

  bool   Negative = false;
  char32 c;

  size_t j, l;

  operator= ("");

  while(i){ // To append is faster that to prefix
    c = (char)(i & 0x0F);
    if(c <= 9){
      c += '0';
    }else{
      c += 'A' - 10;
    }
    Append((char32)c);
    Places--;
    i >>= 4;
  }

  while(Places > 0){
    operator+= ("0");
    Places--;
  }

  if(Negative) Append((char32)'-');

  // Reverse
  l = Length_32 / 2;
  for(j = 0; j < l; j++){
    c = Data_32[j];
    Data_32[j] = Data_32[Length_32-j-1];
    Data_32[Length_32-j-1] = c;
  }
}
//------------------------------------------------------------------------------

void UNICODE_STRING::SetFloat(double d, int SignificantFigures, bool Fill){
  int    shift, places;
  bool   sign, trailing;
  char32 c;
  size_t j, l;
  double f;

  unsigned* u1 = (unsigned*)&f;
  unsigned* u2 = (unsigned*)&d;

  f = 1.0/0.0;
  if(u1[0] == u2[0] && u1[1] == u2[1]){
    operator= ("INF");
    return;
  }
  f = -1.0/0.0;
  if(u1[0] == u2[0] && u1[1] == u2[1]){
    operator= ("-INF");
    return;
  }
  f = 0.0/0.0;
  if(u1[0] == u2[0] && u1[1] == u2[1]){
    operator= ("NAN");
    return;
  }
  if(d == 0.0){
    operator= ("0");
    return;
  }

  operator= ("");

  sign = false;
  if(d < 0.0){
    sign = true;
    d   *= -1.0;
  }

  // Shift so that the whole number is an integer
  f     = pow(10.0, SignificantFigures);
  shift = 0;
  while(round(d) >= f){
    d /= 10.0;
    shift--;
  }
  f /= 10.0;
  while(round(d) < f){
    d *= 10.0;
    shift++;
  }
  d = round(d);

  places   = 0;
  trailing = true;
  while(d >= 1.0){
    if(shift && places == shift){
      if(!trailing) Append((char32)'.');
      trailing = false;
    }
    c = fmod(d, 10.0) + '0';
    if(c > '0' || !trailing || Fill){
    Append(c);
    trailing = false;
    }
    d = floor(d / 10.0);
    places++;
  }

  // Prepend zeros to small numbers
  while(places <= shift){
  if(shift && places == shift) Append((char32)'.');
  Append((char32)'0');
  places++;
  }

  // Prepend sign for negative numbers
  if(sign) Append((char32)'-');

  // Reverse
  l = Length_32 / 2;
  for(j = 0; j < l; j++){
  c = Data_32[j];
  Data_32[j] = Data_32[Length_32-j-1];
  Data_32[Length_32-j-1] = c;
  }

  // Append zeros to large numbers
  while(shift < 0){
  Append((char32)'0');
  shift++;
  }

  // Invalidate the UTF8 and UTF16 buffers
  Invalidate();
}
//------------------------------------------------------------------------------

void UNICODE_STRING::AppendHex(unsigned i, int Places){
  Invalidate();

  UNICODE_STRING s;
  s.SetHex(i, Places);
  operator+= (s);
}
//------------------------------------------------------------------------------

void UNICODE_STRING::AppendFloat(double d, int SignificantFigures, bool Fill){
  Invalidate();

  UNICODE_STRING s;
  s.SetFloat(d, SignificantFigures, Fill);
  operator+= (s);
}
//------------------------------------------------------------------------------

// http://www.info.teradata.com/HTMLPubs/DB_TTU_14_00/index.html#page/
// International_Character_Set/B035_1125_111K/appe.15.10.html
void UNICODE_STRING::UpperCase(){
  size_t j;
  Invalidate();

  for(j = 0; Data_32[j]; j++){
    if     (Data_32[j] >= 0x0061 && Data_32[j] <= 0x007A) Data_32[j] -= 32;
    else if(Data_32[j] >= 0x00E0 && Data_32[j] <= 0x00F6) Data_32[j] -= 32;
    else if(Data_32[j] >= 0x00F8 && Data_32[j] <= 0x00FE) Data_32[j] -= 32;
    else if(Data_32[j] >= 0x023F && Data_32[j] <= 0x0240) Data_32[j] += 10815;
    else if(Data_32[j] >= 0x0256 && Data_32[j] <= 0x0257) Data_32[j] -= 205;
    else if(Data_32[j] >= 0x028A && Data_32[j] <= 0x028B) Data_32[j] -= 217;
    else if(Data_32[j] >= 0x037B && Data_32[j] <= 0x037D) Data_32[j] += 130;
    else if(Data_32[j] >= 0x03AD && Data_32[j] <= 0x03AF) Data_32[j] -= 37;
    else if(Data_32[j] >= 0x03B1 && Data_32[j] <= 0x03C1) Data_32[j] -= 32;
    else if(Data_32[j] >= 0x03C3 && Data_32[j] <= 0x03CB) Data_32[j] -= 32;
    else if(Data_32[j] >= 0x03CD && Data_32[j] <= 0x03CE) Data_32[j] -= 63;
    else if(Data_32[j] >= 0x0430 && Data_32[j] <= 0x044F) Data_32[j] -= 32;
    else if(Data_32[j] >= 0x0450 && Data_32[j] <= 0x045F) Data_32[j] -= 80;
    else if(Data_32[j] >= 0x0561 && Data_32[j] <= 0x0586) Data_32[j] -= 48;
    else if(Data_32[j] >= 0x1F00 && Data_32[j] <= 0x1F07) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F10 && Data_32[j] <= 0x1F15) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F20 && Data_32[j] <= 0x1F27) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F30 && Data_32[j] <= 0x1F37) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F40 && Data_32[j] <= 0x1F45) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F60 && Data_32[j] <= 0x1F67) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F70 && Data_32[j] <= 0x1F71) Data_32[j] += 74;
    else if(Data_32[j] >= 0x1F72 && Data_32[j] <= 0x1F75) Data_32[j] += 86;
    else if(Data_32[j] >= 0x1F76 && Data_32[j] <= 0x1F77) Data_32[j] += 100;
    else if(Data_32[j] >= 0x1F78 && Data_32[j] <= 0x1F79) Data_32[j] += 128;
    else if(Data_32[j] >= 0x1F7A && Data_32[j] <= 0x1F7B) Data_32[j] += 112;
    else if(Data_32[j] >= 0x1F7C && Data_32[j] <= 0x1F7D) Data_32[j] += 126;
    else if(Data_32[j] >= 0x1F80 && Data_32[j] <= 0x1F87) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1F90 && Data_32[j] <= 0x1F97) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1FA0 && Data_32[j] <= 0x1FA7) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1FB0 && Data_32[j] <= 0x1FB1) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1FD0 && Data_32[j] <= 0x1FD1) Data_32[j] += 8;
    else if(Data_32[j] >= 0x1FE0 && Data_32[j] <= 0x1FE1) Data_32[j] += 8;
    else if(Data_32[j] >= 0x2170 && Data_32[j] <= 0x217F) Data_32[j] -= 16;
    else if(Data_32[j] >= 0x24D0 && Data_32[j] <= 0x24E9) Data_32[j] -= 26;
    else if(Data_32[j] >= 0x2C30 && Data_32[j] <= 0x2C5E) Data_32[j] -= 48;
    else if(Data_32[j] >= 0x2D00 && Data_32[j] <= 0x2D25) Data_32[j] -= 7264;
    else if(Data_32[j] >= 0xFF41 && Data_32[j] <= 0xFF5A) Data_32[j] -= 32;

    else if(Data_32[j] == 0x00B5) Data_32[j] += 743;
    else if(Data_32[j] == 0x00FF) Data_32[j] += 121;
    else if(Data_32[j] == 0x0101) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0103) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0105) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0107) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0109) Data_32[j] -= 1;
    else if(Data_32[j] == 0x010B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x010D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x010F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0111) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0113) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0115) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0117) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0119) Data_32[j] -= 1;
    else if(Data_32[j] == 0x011B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x011D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x011F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0121) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0123) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0125) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0127) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0129) Data_32[j] -= 1;
    else if(Data_32[j] == 0x012B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x012D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x012F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0131) Data_32[j] -= 232;
    else if(Data_32[j] == 0x0133) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0135) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0137) Data_32[j] -= 1;
    else if(Data_32[j] == 0x013A) Data_32[j] -= 1;
    else if(Data_32[j] == 0x013C) Data_32[j] -= 1;
    else if(Data_32[j] == 0x013E) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0140) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0142) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0144) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0146) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0148) Data_32[j] -= 1;
    else if(Data_32[j] == 0x014B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x014D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x014F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0151) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0153) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0155) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0157) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0159) Data_32[j] -= 1;
    else if(Data_32[j] == 0x015B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x015D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x015F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0161) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0163) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0165) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0167) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0169) Data_32[j] -= 1;
    else if(Data_32[j] == 0x016B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x016D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x016F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0171) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0173) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0175) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0177) Data_32[j] -= 1;
    else if(Data_32[j] == 0x017A) Data_32[j] -= 1;
    else if(Data_32[j] == 0x017C) Data_32[j] -= 1;
    else if(Data_32[j] == 0x017E) Data_32[j] -= 1;
    else if(Data_32[j] == 0x017F) Data_32[j] -= 300;
    else if(Data_32[j] == 0x0180) Data_32[j] += 195;
    else if(Data_32[j] == 0x0183) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0185) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0188) Data_32[j] -= 1;
    else if(Data_32[j] == 0x018C) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0192) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0195) Data_32[j] += 97;
    else if(Data_32[j] == 0x0199) Data_32[j] -= 1;
    else if(Data_32[j] == 0x019A) Data_32[j] += 163;
    else if(Data_32[j] == 0x019E) Data_32[j] += 130;
    else if(Data_32[j] == 0x01A1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01A3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01A5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01A8) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01AD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01B0) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01B4) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01B6) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01B9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01BD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01BF) Data_32[j] += 56;
    else if(Data_32[j] == 0x01C5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01C6) Data_32[j] -= 2;
    else if(Data_32[j] == 0x01C8) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01C9) Data_32[j] -= 2;
    else if(Data_32[j] == 0x01CB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01CC) Data_32[j] -= 2;
    else if(Data_32[j] == 0x01CE) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01D0) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01D2) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01D4) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01D6) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01D8) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01DA) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01DC) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01DD) Data_32[j] -= 79;
    else if(Data_32[j] == 0x01DF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01E1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01E3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01E5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01E7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01E9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01EB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01ED) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01EF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01F2) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01F3) Data_32[j] -= 2;
    else if(Data_32[j] == 0x01F5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01F9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01FB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01FD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x01FF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0201) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0203) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0205) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0207) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0209) Data_32[j] -= 1;
    else if(Data_32[j] == 0x020B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x020D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x020F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0211) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0213) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0215) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0217) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0219) Data_32[j] -= 1;
    else if(Data_32[j] == 0x021B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x021D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x021F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0223) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0225) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0227) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0229) Data_32[j] -= 1;
    else if(Data_32[j] == 0x022B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x022D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x022F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0231) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0233) Data_32[j] -= 1;
    else if(Data_32[j] == 0x023C) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0242) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0247) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0249) Data_32[j] -= 1;
    else if(Data_32[j] == 0x024B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x024D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x024F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0250) Data_32[j] += 10783;
    else if(Data_32[j] == 0x0251) Data_32[j] += 2588;
    else if(Data_32[j] == 0x0252) Data_32[j] += 10782;
    else if(Data_32[j] == 0x0253) Data_32[j] -= 210;
    else if(Data_32[j] == 0x0254) Data_32[j] -= 206;
    else if(Data_32[j] == 0x0259) Data_32[j] -= 202;
    else if(Data_32[j] == 0x025B) Data_32[j] -= 203;
    else if(Data_32[j] == 0x0260) Data_32[j] -= 205;
    else if(Data_32[j] == 0x0263) Data_32[j] -= 207;
    else if(Data_32[j] == 0x0265) Data_32[j] += 42280;
    else if(Data_32[j] == 0x0268) Data_32[j] -= 209;
    else if(Data_32[j] == 0x0269) Data_32[j] -= 211;
    else if(Data_32[j] == 0x026B) Data_32[j] += 10743;
    else if(Data_32[j] == 0x026F) Data_32[j] -= 211;
    else if(Data_32[j] == 0x0271) Data_32[j] += 10749;
    else if(Data_32[j] == 0x0272) Data_32[j] -= 213;
    else if(Data_32[j] == 0x0275) Data_32[j] -= 214;
    else if(Data_32[j] == 0x027D) Data_32[j] += 10727;
    else if(Data_32[j] == 0x0283) Data_32[j] -= 218;
    else if(Data_32[j] == 0x0288) Data_32[j] -= 218;
    else if(Data_32[j] == 0x0289) Data_32[j] -= 69;
    else if(Data_32[j] == 0x028C) Data_32[j] -= 71;
    else if(Data_32[j] == 0x0292) Data_32[j] -= 219;
    else if(Data_32[j] == 0x0345) Data_32[j] += 84;
    else if(Data_32[j] == 0x0371) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0373) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0377) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03AC) Data_32[j] -= 38;
    else if(Data_32[j] == 0x03C2) Data_32[j] -= 31;
    else if(Data_32[j] == 0x03CC) Data_32[j] -= 64;
    else if(Data_32[j] == 0x03D0) Data_32[j] -= 62;
    else if(Data_32[j] == 0x03D1) Data_32[j] -= 57;
    else if(Data_32[j] == 0x03D5) Data_32[j] -= 47;
    else if(Data_32[j] == 0x03D6) Data_32[j] -= 54;
    else if(Data_32[j] == 0x03D7) Data_32[j] -= 8;
    else if(Data_32[j] == 0x03D9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03DB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03DD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03DF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03E1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03E3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03E5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03E7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03E9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03EB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03ED) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03EF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03F0) Data_32[j] -= 86;
    else if(Data_32[j] == 0x03F1) Data_32[j] -= 80;
    else if(Data_32[j] == 0x03F2) Data_32[j] += 7;
    else if(Data_32[j] == 0x03F5) Data_32[j] -= 96;
    else if(Data_32[j] == 0x03F8) Data_32[j] -= 1;
    else if(Data_32[j] == 0x03FB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0461) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0463) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0465) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0467) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0469) Data_32[j] -= 1;
    else if(Data_32[j] == 0x046B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x046D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x046F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0471) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0473) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0475) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0477) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0479) Data_32[j] -= 1;
    else if(Data_32[j] == 0x047B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x047D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x047F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0481) Data_32[j] -= 1;
    else if(Data_32[j] == 0x048B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x048D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x048F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0491) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0493) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0495) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0497) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0499) Data_32[j] -= 1;
    else if(Data_32[j] == 0x049B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x049D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x049F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04A1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04A3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04A5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04A7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04A9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04AB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04AD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04AF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04B1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04B3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04B5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04B7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04B9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04BB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04BD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04BF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04C2) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04C4) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04C6) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04C8) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04CA) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04CC) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04CE) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04CF) Data_32[j] -= 15;
    else if(Data_32[j] == 0x04D1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04D3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04D5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04D7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04D9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04DB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04DD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04DF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04E1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04E3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04E5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04E7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04E9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04EB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04ED) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04EF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04F1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04F3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04F5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04F7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04F9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04FB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04FD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x04FF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0501) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0503) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0505) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0507) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0509) Data_32[j] -= 1;
    else if(Data_32[j] == 0x050B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x050D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x050F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0511) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0513) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0515) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0517) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0519) Data_32[j] -= 1;
    else if(Data_32[j] == 0x051B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x051D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x051F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0521) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0523) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0525) Data_32[j] -= 1;
    else if(Data_32[j] == 0x0527) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1D79) Data_32[j] += 35332;
    else if(Data_32[j] == 0x1D7D) Data_32[j] += 3814;
    else if(Data_32[j] == 0x1E01) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E03) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E05) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E07) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E09) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E0B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E0D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E0F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E11) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E13) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E15) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E17) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E19) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E1B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E1D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E1F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E21) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E23) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E25) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E27) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E29) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E2B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E2D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E2F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E31) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E33) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E35) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E37) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E39) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E3B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E3D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E3F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E41) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E43) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E45) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E47) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E49) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E4B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E4D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E4F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E51) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E53) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E55) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E57) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E59) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E5B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E5D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E5F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E61) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E63) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E65) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E67) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E69) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E6B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E6D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E6F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E71) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E73) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E75) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E77) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E79) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E7B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E7D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E7F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E81) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E83) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E85) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E87) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E89) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E8B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E8D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E8F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E91) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E93) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E95) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1E9B) Data_32[j] -= 59;
    else if(Data_32[j] == 0x1EA1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EA3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EA5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EA7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EA9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EAB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EAD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EAF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EB1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EB3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EB5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EB7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EB9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EBB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EBD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EBF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EC1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EC3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EC5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EC7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EC9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ECB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ECD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ECF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ED1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ED3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ED5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ED7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1ED9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EDB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EDD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EDF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EE1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EE3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EE5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EE7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EE9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EEB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EED) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EEF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EF1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EF3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EF5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EF7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EF9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EFB) Data_32[j] -= 4097;
    else if(Data_32[j] == 0x1EFD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1EFF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x1F51) Data_32[j] += 8;
    else if(Data_32[j] == 0x1F53) Data_32[j] += 8;
    else if(Data_32[j] == 0x1F55) Data_32[j] += 8;
    else if(Data_32[j] == 0x1F57) Data_32[j] += 8;
    else if(Data_32[j] == 0x1FB3) Data_32[j] += 9;
    else if(Data_32[j] == 0x1FBE) Data_32[j] -= 7205;
    else if(Data_32[j] == 0x1FC3) Data_32[j] += 9;
    else if(Data_32[j] == 0x1FE5) Data_32[j] += 7;
    else if(Data_32[j] == 0x1FF3) Data_32[j] += 9;
    else if(Data_32[j] == 0x214E) Data_32[j] -= 28;
    else if(Data_32[j] == 0x2184) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C61) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C65) Data_32[j] -= 10795;
    else if(Data_32[j] == 0x2C66) Data_32[j] -= 10792;
    else if(Data_32[j] == 0x2C68) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C6A) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C6C) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C73) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C76) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C81) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C83) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C85) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C87) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C89) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C8B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C8D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C8F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C91) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C93) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C95) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C97) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C99) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C9B) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C9D) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2C9F) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CA1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CA3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CA5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CA7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CA9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CAB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CAD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CAF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CB1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CB3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CB5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CB7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CB9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CBB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CBD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CBF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CC1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CC3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CC5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CC7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CC9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CCB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CCD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CCF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CD1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CD3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CD5) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CD7) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CD9) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CDB) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CDD) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CDF) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CE1) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CE3) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CEC) Data_32[j] -= 1;
    else if(Data_32[j] == 0x2CEE) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA641) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA643) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA645) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA647) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA649) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA64B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA64D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA64F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA651) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA653) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA655) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA657) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA659) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA65B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA65D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA65F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA661) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA663) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA665) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA667) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA669) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA66B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA66D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA681) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA683) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA685) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA687) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA689) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA68B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA68D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA68F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA691) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA693) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA695) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA697) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA723) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA725) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA727) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA729) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA72B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA72D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA72F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA733) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA735) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA737) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA739) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA73B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA73D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA73F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA741) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA743) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA745) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA747) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA749) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA74B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA74D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA74F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA751) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA753) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA755) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA757) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA759) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA75B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA75D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA75F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA761) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA763) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA765) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA767) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA769) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA76B) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA76D) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA76F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA77A) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA77C) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA77F) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA781) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA783) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA785) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA787) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA78C) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA791) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA7A1) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA7A3) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA7A5) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA7A7) Data_32[j] -= 1;
    else if(Data_32[j] == 0xA7A9) Data_32[j] -= 1;
  }
}
//------------------------------------------------------------------------------
