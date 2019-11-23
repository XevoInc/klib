/*
Copyright 2019 Xevo Inc.

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

#ifndef XLIB_ERROR_H_
#define XLIB_ERROR_H_

typedef enum {
    XLIB_ERROR_OP_FAILED = -1,
    XLIB_ERROR_BUCKET_EMPTY = -2,
    XLIB_ERROR_ELEM_DELETED = -3
} xlib_error;

#endif /* XLIB_ERROR_H_ */
