# libgo

A Go-like multi-thread asynchronous runtime.

## Pre-Requirement

- [cmake](https://cmake.org)
- [googletest](https://github.com/google/googletest)
- [vcpkg](https://vcpkg.io/) - *Windows Only*


## Build

Run

```bash
mkdir build && cd $_
cmake ..
cmake --build .
```

and you'll get:

 - A shared library `libgo.so` or `libgo.dylib` to be linked to your project
 - An executable `test` to run all the tests.


## Tests

- Like Go, all unit tests must be writen in a file named `xxx_test.c++` alongside a source file `xxx.c` or `xxx.c++`.
- All functional tests must be writen in a file named `xxx_test.c++` under directory `tests` where `xxx` is what to be tested.
