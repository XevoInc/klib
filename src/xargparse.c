/**
 * @file      xargparse.c
 * @brief     Command line argument parser a la Python
 * @author    Vlad Sadovsky <vsadovsky at xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 */

#include <xargparse.h>

extern const char *argp_program_version  = XARG_PROGRAM_VERSION;
extern const char *argp_program_bug_address  = XARG_MAIL_ADDRESS;

static char program_doc[] = "generic xargparse program";
static char args_default_docs[] = "ARG1...";

/*
*  From argp.h
*  long-name, key, descriptive-name,flags,doc-string,group
*  End is marked with NULL long-name
*/
static struct argp_option *argp_l0pt_options;

/* Used to communicate with parsing callback from longopt*/
typedef struct _argp_l0pt_ctx
{
    /* Positional arguments */
    uint    max_pos_args, min_pos_args;
    uint    npos_args;
    char*   pos_args[XARG_MAX_POS_ARGS];
    /* Standard fields */
    bool    verbose, silent;
    /* Per application fields */
} argp_l0pt_ctx;

static argp_l0pt_ctx argp_context;

static error_t argp_l0pt_cb(int key, char *arg, struct argp_state *state);

/* Program descriptor for argp_longopt*/
static struct argp argp_l0pt_desc = {argp_l0pt_options,
                                     argp_l0pt_cb,
                                     program_doc,args_default_docs};


/* Callback for argp_longopt */
static error_t
argp_l0pt_cb (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  argp_l0pt_ctx* ctx = state->input;

  switch (key)
    {
    case 'q': case 's':
      ctx->silent = true;
      break;
    case 'v':
      ctx->verbose = true;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= ctx->maxpos_args) {
          argp_usage (state);
      }

      ctx->npos_args += 1;
      ctx->pos_args[state->arg_num] = arg;

      break;

    case ARGP_KEY_END:
        if (state->arg_num < ctx->min_pos_args) {
            fprintf(stderr," ERROR: not enough positional arguments \n");
            argp_usage (state);
        }
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Initialize longopt context*/
static void
argp_l0pt_init(argp_l0pt_ctx *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->verbose = false;
    ctx->silent = false;
    ctx->min_pos_args = 0;
    ctx->max_pos_args = sizeof(ctx->pos_args) / sizeof(ctx->pos_args[0]);
    ctx->npos_args = 0;
    /* Allocate room for a single positional argument by default*/
}

errno_t xargparse_init(xargparse* self, xargparse*_entries,uint flags)
{
    // Create argp_longopt table
    // initialize argp
    argp_l0pt_init();
}

errno_t xargparse_parse(xargparse* self,int argc, const char **argv)
{
    argp_parse(&argp_l0pt_desc,argc,argv,0,0,&argp_context);
}

errno_t xargparse_destroy(xargparse* self)
{
    // Free argp table
}

