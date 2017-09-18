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

// Use extended XOpen functions
#define __USE_XOPEN_EXTENDED    1
#define _USE_XOPEN              1
#include <xlib/xargparse.h>

const char *argp_program_version  = XARG_PROGRAM_VERSION;
const char *argp_program_bug_address  = XARG_MAIL_ADDRESS;

static char program_doc[] = "generic xargparse program";
static char args_default_docs[] = "ARG1...";

/*
*  From argp.h
*  long-name, key, descriptive-name,flags,doc-string,group
*  End is marked with NULL long-name
*/
static struct argp_option *argp_l0pt_options = nullptr;

/* Use xargparse as a context to communicate with parsing callback from longopt*/
typedef xargparse argp_l0pt_ctx;

static argp_l0pt_ctx* argp_context;

static error_t argp_l0pt_cb(int key, char *arg, struct argp_state *state);

/* Program descriptor for argp_longopt*/
static struct argp argp_l0pt_desc;

/* Define static version of strdup */
static inline
char* mystrdup(const char* s)
{
    char* p = malloc(strlen(s)+1);
    if (p) strcpy(p, s);
    return p;
}

static const char *
prefix_skip(const char *str, const char *prefix)
{
    size_t len = strlen(prefix);
    return strncmp(str, prefix, len) ? nullptr : (char *)(str + len);
}

static int
prefix_cmp(const char *str, const char *prefix)
{
    for (;; str++, prefix++)
        if (!*prefix) {
            return 0;
        } else if (*str != *prefix) {
            return (unsigned char)*prefix - (unsigned char)*str;
        }
}


/* extended (described by the caller) argument parsing */
static error_t
parse_xargument(xargparse* self,int key,char *arg)
{
    const xargparse_entry *ent_cur = self->arguments;
    while ((ent_cur->type != 0) && (ent_cur->key != key))  {
        ent_cur++;
    }

    if (ent_cur->type == 0) {
        /* Entry hadn't been found for the key given - bail*/
        return ENOENT;
    }

    /* Treat bool special */
    if (ent_cur->type == XARGPARSE_TYPE_BOOL) {
        if (arg == nullptr) {
            // XASSERT - argp given us null pointer as an argument, should not ever happen
            return EINVAL;
        }
        if (!strcasecmp(arg, "false")  || !strcmp(arg,"0")) {
            *(bool *)ent_cur->field = false;
            return EOK;
        } else if(!strcasecmp(arg, "true") || !strcmp(arg,"1")) {
            *(bool *)ent_cur->field = true;
            return EOK;
        }
    }

    if (ent_cur->format == nullptr) {
        if (ent_cur->field_size > 1) {
            uint    len_to_copy = max(strlen(arg),(ent_cur->field_size / sizeof(char) - 1));
            strncpy(ent_cur->field,arg,len_to_copy);
        }
    } else {
        /* Format string given */
        if (arg != nullptr) {
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
argp_l0pt_cb (int key, char *arg, struct argp_state *state)
{
  /* Get the input context from argp_parse */
  argp_l0pt_ctx*    ctx = state->input;
  error_t           rc = EOK;

  switch (key)
    {
    case 'q':
        ctx->silent = true;
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
            argp_usage (state);
        }

        // Received next positional argument
        // FIXME CR : is it safe to not do strdump, simply store pointer for the later use ?
        ctx->npos_args += 1;
        ctx->pos_args[state->arg_num] = mystrdup(arg);

      break;

    case ARGP_KEY_END:
        if (state->arg_num < ctx->min_pos_args) {
            fprintf(stderr," ERROR: not enough positional arguments \n");
            argp_usage (state);
        }
      break;

    default:
        /* Not an apriori known type - match to the entries table passed to us to decipher*/
        rc = parse_xargument((xargparse *)ctx,key,arg);
    }
  return (rc == EOK) ? 0 : ARGP_ERR_UNKNOWN;
}

/* Initialize longopt context*/
static void
argp_l0pt_init(argp_l0pt_ctx *ctx)
{
    ctx->verbose = false;
    ctx->silent = false;
    /* Need to implement setting by the caller FIXME*/
    ctx->min_pos_args = 0;
    ctx->max_pos_args = sizeof(ctx->pos_args) / sizeof(ctx->pos_args[0]);
    ctx->npos_args = 0;
    /* Allocate room for a single positional argument by default*/
}


/* API implementation */
errno_t xargparse_init(xargparse* self, xargparse_entry* entries,uint flags)
{
    const xargparse_entry *ent_cur;

    // Initialize xargparse context
    ZERO_CONTEXT(self);
    self->arguments = entries;
    self->flags = flags;
    self->argc = 0;
    self->argv = nullptr;

    /* Preserve counter of entries passed from the caller*/
    self->ent_count = 0;

    ent_cur = self->arguments;
    for (int i = 0;ent_cur->type != 0; i++, ent_cur++ ) {
        self->ent_count += 1;
    }

    memset(&argp_l0pt_desc,0,sizeof(argp_l0pt_desc));
    argp_l0pt_desc.options  = nullptr;
    argp_l0pt_desc.parser   = argp_l0pt_cb;
    argp_l0pt_desc.doc = program_doc;
    argp_l0pt_desc.args_doc = args_default_docs;
    argp_l0pt_desc.children = nullptr;

    /* use xargp descriptor as a callback context */
    argp_context = (argp_l0pt_ctx*)self;

    argp_l0pt_init(argp_context);

    return 0;
}

errno_t xargparse_parse(xargparse* self,int argc, char **argv)
{
    errno_t                 rc;
    struct argp_option*     cur_option;
    const xargparse_entry*  cur_entry;

    /* Create and initialize argp_argp longoptions table */
    if (self->ent_count > 0) {
        argp_l0pt_options = (struct argp_option *)calloc(self->ent_count+1,sizeof(struct argp_option));
        if (argp_l0pt_options == nullptr) {
            return ENOMEM;
        }

        /* Replicate our entries */
        cur_option = argp_l0pt_options;
        cur_entry = self->arguments;
        for (int i = 0; i < self->ent_count; i++, cur_option++,cur_entry++) {
            cur_option->key = cur_entry->key;
            cur_option->name = cur_entry->long_name;
            cur_option->arg = cur_entry->long_name;
            /* Do we need OPTION_ flags beyond OPTION_ARG_OPTIONAL */
            cur_option->flags = cur_entry->flags;
            cur_option->doc = nullptr;
            cur_option->group = 0;
        }
        /* Final entry must be terminating one (key = 0)*/
        cur_option->key = '\0';
    }
    /* Link options descriptor to the options table*/
    argp_l0pt_desc.options = argp_l0pt_options;

    rc = argp_parse(&argp_l0pt_desc,argc,argv,0,0,argp_context);

    /* Free up options table*/
    if (argp_l0pt_options != nullptr) {
        free(argp_l0pt_options);
        argp_l0pt_options = nullptr;
    }

    return rc;
}

errno_t xargparse_destroy(xargparse* self)
{
    // Free argp table
    for (uint i = 0; i < self->npos_args; i++ ) {
        free(self->pos_args[i]);
        self->pos_args[i] = nullptr;
    }
    self->npos_args = 0;

    return 0;
}



