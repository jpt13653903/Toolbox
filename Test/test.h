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

#ifndef test_h
#define test_h
//------------------------------------------------------------------------------

#include "General.h"
//------------------------------------------------------------------------------

void start(const char* message)
{
    info("%s", message); printf("\n");
}
//------------------------------------------------------------------------------

void done()
{
    printf(ANSI_FG_BRIGHT_BLACK
           "----------------------------------------"
           "----------------------------------------\n\n"
           ANSI_RESET);
}
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------

