

#include<stdio.h>
#include "osinfo.h"

void usage(char *argv[])
{
    printf("\n%s <regfile_path> \n", argv[0]);
    
    return;
}

int main(int argc, char *argv[])
{
    char *info = NULL;

    if(argc < 2)
    {
        usage(argv);
        return 0;
    }

    osi_get_os_details(argv[1], &info);     
    
    return 0;
}


