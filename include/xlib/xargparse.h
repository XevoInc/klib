/**
 * @file      xargparse.h
 * @brief     Command line argument parser a la Python
 * @author    Vlad Sadovsky <vsadovsky at xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 */

#pragma once

#ifndef XARGPARSE_H_
#define XARGPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <argp.h>
#include <string.h>

#include <xlib/xtypes.h>


#ifndef XARG_PROGRAM_VERSION
#define XARG_PROGRAM_VERSION "generic-xargparse-client 0.1"
#endif

#ifndef XARG_MAIL_ADDRESS
#define XARG_MAIL_ADDRESS "<dev@xlib.org>"
#endif


/* Maximum length of the argument string - 1 */
#define XARG_SIZE_T     255

/* Maximum number of positional arguments */
#define XARG_MAX_POS_ARGS 15

extern const char *argp_program_version; //  = XARG_PROGRAM_VERSION;
extern const char *argp_program_bug_address; //  = XARG_MAIL_ADDRESS;

/* Externally visible definitions */

/* Allowed types of optional arguments */
typedef enum _xargparse_type
{
    XARGPARSE_TYPE_END,
    XARGPARSE_TYPE_BOOL,
    XARGPARSE_TYPE_INT,
    XARGPARSE_TYPE_UINT,
    XARGPARSE_TYPE_STRING
} xargparse_type;

/* Validation callback per field */
struct _xargparse;
struct _xargparse_entry;

typedef int xargparse_cb(struct _xargparse *self,
                         const struct _xargparse_entry *entry);

/**
 * xargparse entry format
 *
 * type - _END indicates the last entry
 * short name (key).
 * long name
 * field - pointer to the variable with the parsed result
 * default - pointer to the default value
 * callback - called when matching entry is parsed
 * context - context for the callback
 */
typedef struct _xargparse_entry
{
    xargparse_type  type;
    const char      key;
    const char*     long_name;
    void*           field;
    void*           default_val;
    xargparse_cb*   callback;
    void*           context;
    uint            flags;
} xargparse_entry;

typedef enum _xargparse_flags
{
    XARGPARSE_FLAG_NONE = 0
} xargparse_flags;

/* Parsing context */
typedef struct _xargparse
{
    // Provided by the caller
    const xargparse_entry*  arguments;
    xargparse_flags         flags;
    // Internal
    uint                    npos_args;
    char*                   pos_args[XARG_MAX_POS_ARGS];
    int                     argc;
    const char**            argv;
} xargparse;


/* Entry definitions macros */
#define DEFINE_END()       {XARGPARSE_TYPE_END, '\0', nullptr, nullptr, nullptr, nullptr, nullptr,0}
#define DEFINE_BOOL(...)   {XARGPARSE_TYPE_BOOL,__VA_ARGS__ }
#define DEFINE_UINT(...)   {XARGPARSE_TYPE_UINT,__VA_ARGS__ }
#define DEFINE_INT(...)    {XARGPARSE_TYPE_INT,__VA_ARGS__ }
#define DEFINE_STRING(...) {XARGPARSE_TYPE_STRING,__VA_ARGS__ }
#define DEFINE_DOUBLE(...) {XARGPARSE_TYPE_DOUBLE,__VA_ARGS__ }

/* API */
errno_t xargparse_init(xargparse* self, xargparse_entry* entries ,uint flags);
errno_t xargparse_parse(xargparse* self,int argc, char **argv);
errno_t xargparse_destroy(xargparse* self);

#ifdef __cplusplus
}
#endif

#endif /* XARGPARSE_H_ */
