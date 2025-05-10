### LIBTRACKMEM - Lightweight Runtime Memory Access Tracking for C/C++

Libtrackmem is a minimal C/C++ library that helps developers track memory usage and access patterns at runtime - without bulky instrumentation tools like Valgrind, and without modifying the compiler or replacing standard memory APIs.

---

### Quick Start:

```
git clone https://github.com/gc1523/libtrackmem.git
cd libtrackmem
make install
```

And you're ready to go!

### Usage:

 - Simply use `#include <trackmem.h>` as you would any normal header file.
 - Your file will automagically make use of tracking memory functions.
 - When your program finishes executing, a usage summary will be displayed.
 - Detailed logs are written to a file (default or user-defined).

### Environment Variables:

  The `trackmem` library supports the following environment variables to control its behaviour:

  - `TRACKMEM_DISABLE` - if set to `"1"`, disables memory tracking. Useful if you want to quickly toggle the tracking of your memory usage. Set to `"0"` for default tracking behaviour.
  - `TRACKMEM_LOG` Specifies the name of the log file where memory tracking information will be written. If not set a default filepath will be used.


