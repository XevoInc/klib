/**
 * @file      xargparse.h
 * @brief     Command line argument parser a la Python
 *
 *            Semantics is modeled after Python argparse, except
 *            command line options are described declaratively,
 *            not by the set of function calls.
 *
 *            To describe options declare an array of
 *            structures, using following macros: #define

 *            DEFINE_BOOL(k,nm,f,fl) : boolean option
 *            key (char),long name, backing variable, flags
 *
 *            DEFINE_UINT(k,nm,f,fl) : unsigned integer key
 *            (char),long name, backing variable, flags
 *
 *            DEFINE_INT(k,nm,f,fl)  : integer
 *            key (char),long name, backing variable, flags
 *
 *            DEFINE_DOUBLE(k,nm,f,fl): floating point double
 *            key (char),long name, backing variable, flags
 *
 *            DEFINE_STRING(k,nm,f,sz,fl):null terminated string
 *            key,long name, backing variable,sizeof of it,flags
 *
 *            DEFINE_END() : terminator, final line
 *
 *            Command line should conform to GNU syntax rules
 *            with either short or long names for options.
 *
 *            Positional arguments, up to the MAX value are
 *            placed into an array of strings upton return
 *
 * @author    Vlad Sadovsky <vsadovsky at xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 */

#ifndef XARGPARSE_H_
#define XARGPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

// TODO: move to C as much as possible
#include <argp.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef XARG_PROGRAM_VERSION
#define XARG_PROGRAM_VERSION "generic-xargparse-client 0.1"
#endif

#ifndef XARG_MAIL_ADDRESS
#define XARG_MAIL_ADDRESS "<dev@xlib.org>"
#endif

/* Maximum number of positional arguments */
// TODO: let user specify
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
 * type - _END indicates the last entry
 * short name (key).
 * long name
 * field - pointer to the variable with the parsed result
 * default - pointer to the default value
 * callback - called when matching entry is parsed
 * context - context for the callback
 */
// TODO: const char *p or const char* p but not a mix
typedef struct xargparse_entry
{
    xargparse_type  type;
    const char      key;
    const char*     long_name;
    void*           field;
    unsigned int    field_size;
    char*           format;
    unsigned int    format_len;
    unsigned int    flags;
} xargparse_entry;

/* TODO: Could add negation or more general concept of flags with bitmasks */

// TODO: Perhaps it would be cleaner to hide the struct and have the public fields passed into the function rather than passing the whole struct. Then the caller can't mess up rather than being advised not to touch certain fields. Alternatively, could split into a public and a private struct and commit only to the private struct.
// TODO: can use const char * pointing into argv for large arguments, not
// mallocing, etc.

/* Parsing context */
typedef struct xargparse
{
    /* Provided by the caller */
    const xargparse_entry* arguments;
    unsigned int           ent_count;
    /* Internal */
    int                    argc;
    const char**           argv;
    /* Positional arguments */
    unsigned int           max_pos_args, min_pos_args;
    unsigned int           npos_args;
    char*                  pos_args[XARG_MAX_POS_ARGS];
    /* Standard fields */
    bool                   verbose;
} xargparse;

// TODO: consistent spacing everywhere

/* Entry definitions macros  */
#define DEFINE_END()              {XARGPARSE_TYPE_END, '\0', NULL, NULL,0, NULL,0,0}
#define DEFINE_BOOL(k,nm,f,fl)    {XARGPARSE_TYPE_BOOL,k,nm,&f,sizeof(bool), "%1d",4,fl}
#define DEFINE_UINT(k,nm,f,fl)    {XARGPARSE_TYPE_UINT,k,nm,&f,sizeof(uint),"%4d",4,fl}
#define DEFINE_INT(k,nm,f,fl)     {XARGPARSE_TYPE_INT,k,nm,&f,sizeof(uint),"%4d",4,fl}
#define DEFINE_DOUBLE(k,nm,f,fl)  {XARGPARSE_TYPE_DOUBLE,k,nm,&f,sizeof(double),"%8.8f",6,fl}
#define DEFINE_STRING(k,nm,f,sz,fl) {XARGPARSE_TYPE_STRING,k,nm,f,sz,"%s",2, fl}

typedef int xargparse_err;

/* API */
xargparse_err xargparse_init(xargparse* self, xargparse_entry* entries);
xargparse_err xargparse_parse(xargparse* self,int argc, char **argv);
xargparse_err xargparse_destroy(xargparse* self);

#ifdef __cplusplus
}
#endif

#endif /* XARGPARSE_H_ */
