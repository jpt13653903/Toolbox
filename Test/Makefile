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

CXX     = g++
Options = -std=c++11 -Wall -fexceptions -O2 -DDEBUG
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
          obj/UTF_Converter.o \
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
     bin/testJSON.exe          \
     bin/testUTF_Converter.exe \
     bin/testXML.exe

test: testCalculator    \
      testDebugMessages \
      testDictionary    \
      testFileWrapper   \
      testJSON          \
      testUTF_Converter \
      testXML

test%: bin/test%.exe
	mkdir -p testOutput
	$<

clean:
	rm -rf obj
	rm -rf bin
	rm -rf testOutput
#-------------------------------------------------------------------------------

# Binaries

bin/%.exe: %.cpp $(Objects) $(Headers)
	mkdir -p bin
	$(CXX) $(Options) $(Version) $(Includes) $(Objects) $< -s $(LibInclude) $(Libraries) -o $@
#-------------------------------------------------------------------------------

# Objects

obj/%.o: $(Toolbox)/%.cpp $(Headers)
	mkdir -p $(@D)
	$(CXX) $(Options) $(Version) -c $(Includes) $< -o $@
#-------------------------------------------------------------------------------

