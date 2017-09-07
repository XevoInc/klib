# xlib

## Overview

xlib is a standalone and lightweight C library. It was originally forked from
klib but has since been modified and had various components added and removed.
Most components are independent of external libraries, except the standard C
library, and independent of each other.

See [klib](https://github.com/attractivechaos/klib) for more information about
the components deriving from klib.

## Components

* [khash.h][include/xlib/xhash.h]: generic hash table based on double hashing.
* kvec.h: generic dynamic array.
