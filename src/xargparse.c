/**
 * @file      xargparse.c
 * @brief     Command line argument parser a la Python
 *  Restrictions:
 *      - not reentrant
 *      - not entirely safe wrt caller passed formats CR
 *
 * @author    Vlad Sadovsky <vsadovsky at xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 */

#define     _POSIX_C_SOURCE     200810L

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <xlib/xassert.h>
#include <xlib/xargparse.h>

/* Maximum length of description strings */
#define MAX_STRING  100

/* Error codes */
#define EOK                 0

/* Macros static */
#define ZERO_MEM(p)  (memset(p, 0, sizeof(*p)))
#define SAFE_FREE(p) if (p) { free(0); p=NULL;}
#define NUM_ELEMS(a) (sizeof(a) / sizeof(a[0]))

typedef unsigned int uint;


const char *argp_program_version = NULL;
const char *argp_program_bug_address = NULL;

static const char program_default_doc[] = "generic xargparse program";
static const char args_default_docs[] = "ARG1...";

//static char program_doc[MAX_STRING] ;
//static char args_default_docs[MAX_STRING] ;

/*
*  From argp.h
*  long-name, key, descriptive-name,flags,doc-string,group
*  End is marked with NULL long-name
*/
static struct argp_option *argp_l0pt_options = NULL;

/* Use xargparse as a context to communicate with parsing callback from longopt*/
typedef xargparse argp_l0pt_ctx;

static argp_l0pt_ctx *argp_context;

static error_t argp_l0pt_cb(int key, char *arg, struct argp_state *state);

/* Program descriptor for argp_longopt*/
static struct argp argp_l0pt_desc;

static inline uint
max(uint a, uint b)
{
    return ((a) > (b)) ? (a) : (b);
}

/* extended (described by the caller) argument parsing */
static error_t
parse_xargument(xargparse *self, int key, char *arg)
{
    const xargparse_entry *ent_cur = self->arguments;
    while ((ent_cur->type != 0) && (ent_cur->key != key)) {
        ent_cur++;
    }

    if (ent_cur->type == 0) {
        /* Entry hadn't been found for the key given - bail*/
        return ENOENT;
    }

    /* Treat bool special */
    if (ent_cur->type == XARGPARSE_TYPE_BOOL) {
        if (arg == NULL) {
            return EINVAL;
        }
        if (!strcasecmp(arg, "false") || !strcmp(arg, "0")) {
            *((bool *)ent_cur->field) = false;
            return EOK;
        } else if (!strcasecmp(arg, "true") || !strcmp(arg, "1")) {
            *((bool *)ent_cur->field) = true;
            return EOK;
        }
    }

    if (ent_cur->format == NULL) {
        if (ent_cur->field_size > 1) {
            uint len_to_copy = max(strlen(arg), (ent_cur->field_size / sizeof(char) - 1));
            strncpy(ent_cur->field, arg, len_to_copy);
        }
    } else {
        /* Format string given */
        if (arg != NULL) {
            sscanf(arg, ent_cur->format, ent_cur->field);
        } else {
            // XASSERT - argp given us null pointer as an argument, should not ever happen
            return EINVAL;
        }
    }

    return 0;
}


/* Callback for argp_longopt */
static error_t
argp_l0pt_cb(int key, char *arg, struct argp_state *state)
{
    /* Get the input context from argp_parse */
    argp_l0pt_ctx *ctx = state->input;
    error_t rc = EOK;

    switch (key) {
    case 'q':
        ctx->verbose = false;
        break;
    case 'v':
        ctx->verbose = true;
        break;

    case ARGP_KEY_INIT:
    case ARGP_KEY_ARGS:
    case ARGP_KEY_SUCCESS:
    case ARGP_KEY_NO_ARGS:
        rc = 0;
        break;

    case ARGP_KEY_ERROR:
        rc = 0;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= ctx->max_pos_args) {
            argp_usage(state);
        }

        // Received next positional argument
        ctx->npos_args += 1;
        ctx->pos_args[state->arg_num] = arg;

        break;

    case ARGP_KEY_END:
        if (state->arg_num < ctx->min_pos_args) {
            fprintf(stderr, " ERROR: not enough positional arguments \n");
            argp_usage(state);
        }
        break;

    default:
        /* Not an apriori known type - match to the entries table passed to us to decipher*/
        rc = parse_xargument((xargparse *)ctx, key, arg);
    }
    return (rc == EOK) ? 0 : ARGP_ERR_UNKNOWN;
}

/* Initialize longopt context*/
static void
argp_l0pt_init(argp_l0pt_ctx *ctx)
{
    ctx->verbose = false;
    ctx->min_pos_args = 0;
    ctx->max_pos_args = sizeof(ctx->pos_args) / sizeof(ctx->pos_args[0]);
    ctx->npos_args = 0;
    /* Allocate room for a single positional argument by default*/
}


/* API implementation */
xargparse_err xargparse_init(xargparse *self, xargparse_entry *entries,
                             const char *prg_version, const char *bug_addr,
                             const char *prg_doc, const char *args_doc)
{
    const xargparse_entry *ent_cur;

    /* Initialize xargparse context */
    /* ZERO_MEM(self); is somewhat debatable */
    self->arguments = entries;
    self->max_pos_args = NUM_ELEMS(self->pos_args);
    self->min_pos_args = 0;
    self->verbose = false;

    for (size_t i = 0; i < self->max_pos_args; i++) {
        self->pos_args[0] = NULL;
    }

    /* Preserve counter of entries passed from the caller*/
    self->ent_count = 0;

    ent_cur = self->arguments;
    for (size_t i = 0; ent_cur->type != 0; i++, ent_cur++) {
        self->ent_count += 1;
    }

    ZERO_MEM(&argp_l0pt_desc);
    argp_l0pt_desc.options = NULL;
    argp_l0pt_desc.parser = argp_l0pt_cb;
    argp_l0pt_desc.doc = (prg_doc != NULL) ? prg_doc : program_default_doc;
    argp_l0pt_desc.args_doc = (args_doc != NULL) ? args_doc : args_default_docs;
    argp_l0pt_desc.children = NULL;

    if (prg_version != NULL) {
        argp_program_version = strdup(prg_version);
    }

    if (bug_addr) {
        argp_program_bug_address = strdup(bug_addr);
    }

    /* use xargp descriptor as a callback context */
    argp_context = (argp_l0pt_ctx *)self;

    argp_l0pt_init(argp_context);

    return 0;
}

xargparse_err xargparse_parse(xargparse *self, int argc, char **argv)
{
    xargparse_err rc;
    struct argp_option *cur_option;
    const xargparse_entry *cur_entry;

    /* Create and initialize argp_argp longoptions table */
    if (self->ent_count > 0) {
        argp_l0pt_options = malloc(
            (self->ent_count + 1) * sizeof(*argp_l0pt_options));
        if (argp_l0pt_options == NULL) {
            return ENOMEM;
        }

        /* Replicate our entries */
        cur_option = argp_l0pt_options;
        cur_entry = self->arguments;
        for (size_t i = 0; i < self->ent_count; i++, cur_option++, cur_entry++) {
            cur_option->key = cur_entry->key;
            cur_option->name = cur_entry->long_name;
            cur_option->arg = cur_entry->long_name;
            /* Do we need OPTION_ flags beyond OPTION_ARG_OPTIONAL */
            cur_option->flags = cur_entry->flags;
            cur_option->doc = NULL;
            cur_option->group = 0;
        }
        /* Final entry must be terminating one (key = 0)*/
        cur_option->key = '\0';
    }
    /* Link options descriptor to the options table*/
    argp_l0pt_desc.options = argp_l0pt_options;

    rc = argp_parse(&argp_l0pt_desc, argc, argv, 0, 0, argp_context);

    /* Free up options table*/
    if (argp_l0pt_options != NULL) {
        free(argp_l0pt_options);
        argp_l0pt_options = NULL;
    }

    return rc;
}

xargparse_err xargparse_destroy(xargparse *self)
{
    // Free argp table
    for (uint i = 0; i < self->npos_args; i++) {
        self->pos_args[i] = NULL;
    }
    self->npos_args = 0;

    SAFE_FREE(argp_program_version);
    SAFE_FREE(argp_program_bug_address);

    return 0;
}
