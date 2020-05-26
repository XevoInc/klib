/*
Copyright 2020 Xevo Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

<http://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef XLIB_PRIVATE_ALLOC_H_
#define XLIB_PRIVATE_ALLOC_H_

/* Allocation functions used by the rest of xlib. */

#ifndef xroundup32
#define xroundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#endif

#ifndef xcalloc
#define xcalloc(N,Z) calloc(N,Z)
#endif
#ifndef xmalloc
#define xmalloc(Z) malloc(Z)
#endif
#ifndef xrealloc
#define xrealloc(P,Z) realloc(P,Z)
#endif
#ifndef xfree
#define xfree(P) free(P)
#endif

#endif /* XLIB_PRIVATE_ALLOC_H_ */
