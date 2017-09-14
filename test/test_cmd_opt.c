#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
//
// Command line options and their default values.
//
#define CMD_OPT_ONE_LOCAL_COPY 1
#include <xlib/cmdopt_parser.h>
#undef  CMD_OPT_ONE_LOCAL_COPY

////////////////////////////////////////////////////////////////////////////////////////
//
//              variable name
//              |              single-letter option
//              |              |    --long style option name
//              |              |    |               option value name/format (if any)
//              |              |    |               |              default val  validator fn.  option description
//              vvvvvvvvvvvvv  vvv  vvvvvvvvvvvvvv  vvvvvvvvvvvvv  vvvvvvvvvvv  vvvvvvvvvvvvv  vvvvvvvvvvvvvvvvvv...
//------------------------------------------------------------------------------------------------------------------
CMDOPT_DEFINE_U(verbose     , 'v', "verbose"     ,     "[level]",        0,       NULL, "Verbosity level");

////////////////////////////////////////////////////////////////////////////////////////

extern void usage() // might be called from the command-line option parser
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "    test_cmd_opt [ option(s) ] [ _file_path ]\n");
  fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{

     /* Skip over our own path */
  argc--;
  argv++;

  /* Process command-line options */
  if (!processCmdOpts(&argc, (const char ***)&argv)) {
    fprintf(stderr, "ERROR: processing command line failed \n");
    exit(2);
  }

  printf("Command line flags: \n");
  printf("\tFLAG_verbose=%d \n",FLAG_verbose);

  exit(0);
}


