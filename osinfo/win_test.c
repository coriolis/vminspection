
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include "osinfo.h"

void usage(char *argv[])
{
    printf("\n%s <regfile_path> \n", argv[0]);
    
    return;
}

static char *regfilename = NULL;

int myopen(char *fname, int mod)
{
    fprintf(stderr, "Opening %s \n", regfilename);
    return open(regfilename, mod);
}

int myread(int fd, char *buf, int size)
{
    int ret =0;
    ret = read(fd, buf, size);
    if(ret <0)
        perror("Failed to read ....\n");
    fprintf(stderr, "Read %d asked %d \n", ret, size);

    return ret;
}
int mylseek(int fd, off_t off, int wh)
{
    int ret =0;
    ret = lseek(fd, off, wh);
    if(ret <0)
        perror("Failed to read ....\n");
    fprintf(stderr, "lseek from %d off %d \n", wh, (int)off);

    return ret;
}

int main(int argc, char *argv[])
{
    char **info = NULL;
    int i=0;

    if(argc < 2)
    {
        usage(argv);
        return 0;
    }

    regfilename = argv[1];
    osi_get_os_details(myopen, myread, mylseek, &info);     
    while(info && info[i])
    {
        fprintf(stderr, "\t%s: %s\n", info[i], info[i+1]);
        free(info[i]);
        free(info[i+1]);
        i+=2;
    }
    
    free(info);
    return 0;
}


