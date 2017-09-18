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
#include <strings.h>

#include <xlib/xtypes.h>

#ifndef XARG_PROGRAM_VERSION
#define XARG_PROGRAM_VERSION "generic-xargparse-client 0.1"
#endif

#ifndef XARG_MAIL_ADDRESS
#define XARG_MAIL_ADDRESS "<dev@xlib.org>"
#endif

/* Maximum length of the argument string - 1 and a single argument*/
#define XARG_SIZE_MAX     1023
#define XARG_OPTSIZE_MAX  255

/* Maximum number of positional arguments */
#define XARG_MAX_POS_ARGS 15

/* Error codes */
#define EOK                 0

/* Macros static */
#define ZERO_CONTEXT(p)  (memset(p,0,sizeof(*p)))

/* Define if not standard */
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

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
 *strcmpi
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
    uint            field_size;
    char*           format;
    uint            format_len;
    uint            flags;
    #ifdef NOT_IMPLEMENTED
    /* caller provided validation callbacks */
    xargparse_cb*   callback;
    void*           context;
    #endif //NOT_IMPLEMENTED

} xargparse_entry;

typedef enum _xargparse_flags
{
    XARGPARSE_FLAG_NONE = 0
} xargparse_flags;

/* Parsing context */
typedef struct _xargparse
{
    /* Provided by the caller */
    const xargparse_entry*  arguments;
    xargparse_flags         flags;
    int                     ent_count;
    /* Internal */
    int                     argc;
    const char**            argv;
    /* Positional arguments */
    uint    max_pos_args, min_pos_args;
    uint    npos_args;
    char*   pos_args[XARG_MAX_POS_ARGS];
    /* Standard fields */
    bool    verbose, silent;
} xargparse;

/* Entry definitions macros  */
#define DEFINE_END()              {XARGPARSE_TYPE_END, '\0', nullptr, nullptr,0, nullptr,0,0}
#define DEFINE_BOOL(k,nm,f,fl)    {XARGPARSE_TYPE_BOOL,k,nm,&f,sizeof(bool),"%1d",4,fl}
#define DEFINE_UINT(k,nm,f,fl)    {XARGPARSE_TYPE_UINT,k,nm,&f,sizeof(uint),"%4d",4,fl}
#define DEFINE_INT(k,nm,f,fl)     {XARGPARSE_TYPE_INT,k,nm,&f,sizeof(uint),"%4d",4,fl}
#define DEFINE_DOUBLE(k,nm,f,fl)  {XARGPARSE_TYPE_DOUBLE,k,nm,&f,sizeof(double),"%8.8f",6,fl}
#define DEFINE_STRING(k,nm,f,sz,fl) {XARGPARSE_TYPE_STRING,k,nm,f,sz,"%s",2, fl}

/* API */
errno_t xargparse_init(xargparse* self, xargparse_entry* entries ,uint flags);
errno_t xargparse_parse(xargparse* self,int argc, char **argv);
errno_t xargparse_destroy(xargparse* self);

#ifdef __cplusplus
}
#endif

#endif /* XARGPARSE_H_ */
