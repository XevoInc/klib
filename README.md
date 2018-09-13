# xlib

## Overview

xlib is a standalone and lightweight C library. It was originally forked from
klib but has since been modified and had various components added and removed.
Most components are independent of external libraries, except the standard C
library, and independent of each other.

See [klib](https://github.com/attractivechaos/klib) for more information about
the components deriving from klib.

## Components

* xargparse: generic command-line argument parsing.
* xassert: generic macro-based assertions.
* xhash: generic hash table based on double hashing.
* xvec: generic dynamic array.
