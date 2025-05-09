### LIBTRACKMEM - Lightweight Runtime Memory Access Tracking for C/C++

Libtrackmem is a minimal C/C++ library that helps developers track memory usage and access patterns at runtime — without bulky instrumentation tools like Valgrind, and without modifying the compiler or replacing standard memory APIs.

Quick Start:

  - `git clone https://github.com/gc1523/libtrackmem.git`
  - `cd libtrackmem`
  - `make install`
  - And you're all set!

Usage:

 - Simply use `#include <trackmem.h>` as you would any normal header file
 - Your file will automagically make use of tracking memory functions
 - When your program finishes executing, a summary will be displayed
 - You can view full logs in linked file
