# Toolbox

General-purpose repository, intended as a submodule for others.

## Unit Tests and Examples

- To see usage examples, look at the [Test](Test) folder.
- To run the tests, run `make test` from the [Test](Test) folder.

## Cpp Folder

- **Calculator.cpp**
    - The engine used in [EngCalc](https://sourceforge.net/p/alwaysontopcalc/wiki).
- **Dictionary.cpp**
    - A [left-leaning red-black tree](https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf) used to store key-value pairs: the key is a string and the value type is templated.
- **FileWrapper.cpp**
    - Abstraction for reading and writing files, as well as obtaining and modifying the various time-stamps.
- **General.cpp**
    - Provides OS-independence and includes often-used standard headers.
    - Defines `kiB` (2<sup>10</sup>) and `MiB` (2<sup>20</sup>) constants.
    - Defines macros for standard ANSI escape sequences related to changing the console text colour.
    - Defines `debug`, `info`, `warning` and `error` macros.  These are syntactically identical to `printf`, but automatically adds colours and more information relating to the current file, line number and function name.
- **JSON.cpp**
    - Abstraction for reading, manipulating and generating JSON strings.  It supports parsing of [JSON-5](https://json5.org/) strings, but stringifies to normal JSON.
- **LLRBTree.cpp**
    - A general-purpose [left-leaning red-black tree](https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf) used to store objects.
- **UTF\_Converter.cpp**
    - Utility used to convert between UTF-8 (std::string), UTF-16 (std::u16string) and UTF-32 (std::u32string).
- **XML.cpp**
    - Abstraction for reading and writing XML files.

