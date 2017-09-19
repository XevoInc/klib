#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xlib/xargparse.h>

typedef unsigned int uint;

int main(int argc, char *argv[])
{
    int     itest = 0;
    uint    uitest = 0;
    bool    btest = false;
    char    stest[100] = "\0";

    xargparse_entry xe[] =
    {
        /* for non strings : key , longname, variable, flag */
        /* for strings: key, longname, variable, sizeof, flag*/
        DEFINE_BOOL('b',"btest",btest,0),
        DEFINE_INT('i',"itest",itest,OPTION_ARG_OPTIONAL),
        DEFINE_UINT('u',"uitest",uitest,OPTION_ARG_OPTIONAL),
        DEFINE_STRING('s',"stest",stest,sizeof(stest),OPTION_ARG_OPTIONAL),
        DEFINE_END()
    };

    xargparse xa;

    printf("\n>>Test for extended argument parsing \n\n");

    xargparse_init(&xa, xe);
    xargparse_parse(&xa, argc, argv);

    printf("Command line flags: \n"
           "\titest=%d\n\tbtest=%d\n\tuitest=%d\n\tstest=%s\n",
           itest,btest,uitest,stest);
    printf("\nPositional arguments:\tcount=%d \n", xa.npos_args);
    for (uint i = 0; i < xa.npos_args; i++) {
        printf("\t[%-2d]=%s\n",i+1,xa.pos_args[i]);
    }

    xargparse_destroy(&xa);
    exit(0);
}


