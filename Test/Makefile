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
          obj/FileWrapper.o   \
          obj/JSON.o          \
          obj/LLRBTree.o      \
          obj/General.o       \
          obj/UnicodeString.o \
          obj/XML.o

Headers = *.h \
          $(Toolbox)/*.h
#-------------------------------------------------------------------------------

.PHONY:    clean all
.PRECIOUS: $(Objects)

all: bin/testDebugMessages.exe \
     bin/testFileWrapper.exe   \
     bin/testCalculator.exe    \
     bin/testJSON.exe

test: all
	bin/testDebugMessages.exe
	bin/testFileWrapper.exe
	bin/testCalculator.exe
	bin/testJSON.exe

clean:
	rm -rf obj
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

