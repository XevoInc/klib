#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xlib/xargparse.h>

int main(int argc, char *argv[])
{
    int     itest = 0;
    uint    uitest = 0;
    bool    btest = false;
    char    stest[100] = "\0";

    xargparse_entry xe[] =
    {
        DEFINE_BOOL('b',"btest",&btest,nullptr,nullptr,nullptr,0),
        DEFINE_INT('i',"itest",&itest,nullptr,nullptr,nullptr,0),
        DEFINE_UINT('u',"uitest",&uitest,nullptr,nullptr,nullptr,0),
        DEFINE_STRING('s',"stest",&stest,"",nullptr,nullptr,0),
        DEFINE_END()
    };

    xargparse xa;

    xargparse_init(&xa,xe,0);
    argc = xargparse_parse(&xa,argc,argv);


    printf("Command line flags: \n"
           "\titest=%d \n",itest
           );

    exit(0);
}


