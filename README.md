# cutils - Frequently used utilities for the C programming language

This is a collection of utilities for the C programming language:

- UT - A simple unit test framework
- Err - Simple error reporting functions
- AList - An array list implementation
- HMap - A hash map implementation

## How to use it?

1. Clone the repo
2. Build with `make all`
3. Test with `make tst` if build was successful
4. To use it, include headers from `include` and link `lib/ctutls.a` to your executable

## What is in it?

### UT - A unit test framework

To use UT, simply include `ut.h`. That'a all. For examples and a usage
description, look at `ut.h`.

### AList - An array list implementation

For examples and usage description, look at `alist.h`.

### HMap - A hash map implementation

For examples and usage description, look at `hmap.h`.

### Err - Simple error reporting functions

For examples and usage description, look at `err.h`.
