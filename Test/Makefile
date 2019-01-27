#===============================================================================
# Copyright (C) John-Philip Taylor
# jpt13653903@gmail.com
#
# This file is part of a library
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#===============================================================================

CC      = g++
Options = -Wall -fexceptions -O2 -DDEBUG
#-------------------------------------------------------------------------------

Toolbox = ../Cpp
#-------------------------------------------------------------------------------

Includes   = -I$(Toolbox)
Libraries  = 
LibInclude = 
#-------------------------------------------------------------------------------

Version = -DMAJOR_VERSION=1 -DMINOR_VERSION=0

Objects = obj/Calculator.o    \
          obj/Dictionary.o    \
          obj/FileWrapper.o   \
          obj/JSON.o          \
          obj/LLRBTree.o      \
          obj/General.o       \
          obj/UnicodeString.o \
          obj/XML.o

Headers = *.h \
          $(Toolbox)/*.h
#-------------------------------------------------------------------------------

.PHONY: clean all
.SECONDARY:

all: bin/testCalculator.exe    \
     bin/testDebugMessages.exe \
     bin/testDictionary.exe    \
     bin/testFileWrapper.exe   \
     bin/testJSON.exe

test: testCalculator    \
      testDebugMessages \
      testDictionary    \
      testFileWrapper   \
      testJSON

test%: bin/test%.exe
	$<

clean:
	rm -rf obj
	rm -rf bin
#-------------------------------------------------------------------------------

# Binaries

bin/%.exe: %.cpp $(Objects) $(Headers)
	mkdir -p bin
	$(CC) $(Options) $(Version) $(Includes) $< $(Objects) -s $(LibInclude) $(Libraries) -o $@
#-------------------------------------------------------------------------------

# Objects

obj/%.o: $(Toolbox)/%.cpp $(Headers)
	mkdir -p $(@D)
	$(CC) $(Options) $(Version) -c $(Includes) $< -o $@
#-------------------------------------------------------------------------------

