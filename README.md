# Toolbox

General-purpose repository, intended as a submodule for others.

## Unit Tests and Examples

- To see usage examples, look at the [Test](Test) folder.
- To run the tests, run `make test` from the [Test](Test) folder.

## Cpp Folder

- **Calculator.cpp**
    - The engine used in [EngCalc](https://sourceforge.net/p/alwaysontopcalc/wiki).
- **Dictionary.cpp**
    - A [left-leaning red-black tree](https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf) used to store key-value pairs: the key is a string and the value is a void-pointer.
- **FileWrapper.cpp**
    - Abstraction for reading and writing files, as well as obtaining and modifying the various time-stamps.
- **General.cpp**
    - Provides OS-independence and includes often-used standard headers.
    - Defines `kiB` (2<sup>10</sup>) and `MiB` (2<sup>20</sup>) constants.
    - Defines macros for standard ANSI escape sequences related to changing the console text colour.
    - Defines `debug`, `info`, `warning` and `error` macros.  These are syntactically identical to `printf`, but automatically adds colours and more information relating to the current file, line number and function name.
- **JSON.cpp**
    - Abstraction for reading, manipulating and generating JSON strings.
    - The implementation is not time-optimal &ndash; do not use as a general-purpose dynamic data structure.
- **LLRBTree.cpp**
    - A general-purpose [left-leaning red-black tree](https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf) used to store objects.
- **UnicodeString.cpp**
    - _Deprecated - do not use for new designs_.
    - Abstraction that stores strings in UTF-32, with conversion to- and from UTF-16 and UTF-8.
- **XML.cpp**
    - Abstraction for reading and writing XML files.

