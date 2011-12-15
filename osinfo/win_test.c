
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include "osinfo.h"

void usage(char *argv[])
{
    printf("\n%s <regfile_path> \n", argv[0]);
    
    return;
}

static char *regfilename = NULL;
static int dumpfd =0;
static int readcount = 0;

int myopen(char *fname, int mod)
{
    fprintf(stderr, "Opening %s \n", regfilename);
    //dumpfd = open("/tmp/dump1", O_CREAT|O_WRONLY|O_TRUNC, 077);
    return open(regfilename, mod);
}

int myread(int fd, char *buf, int size, size_t off)
{
    int ret =0;
    ret = pread(fd, buf, size, off);
    if(ret <0)
        perror("Failed to read ....\n");
    readcount += ret;
    if(dumpfd) {
        pwrite(dumpfd, buf, ret, off);
    }
    fprintf(stderr, "Read %d asked %d \n", ret, size);

    return ret;
}
size_t mysize(int fd)
{
    struct stat statbuf;
    if (fstat (fd, &statbuf) == -1)
        perror("Failed to stat ....\n");
    fprintf(stderr, "Size : %ld \n", statbuf.st_size);

    return statbuf.st_size;

}
int mylseek(int fd, off_t off, int wh)
{
    int ret =0;
    ret = lseek(fd, off, wh);
    if(ret <0)
        perror("Failed to read ....\n");
    fprintf(stderr, "Seek off %d wh %d \n", (int)off, wh);

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
    osi_get_os_details(myopen, myread, mysize, &info);     
    while(info && info[i])
    {
        fprintf(stderr, "\t%s: %s\n", info[i], info[i+1]);
        free(info[i]);
        free(info[i+1]);
        i+=2;
    }
    fprintf(stderr, "Total read %d \n", readcount);
    if(dumpfd) close(dumpfd);
    
    free(info);
    return 0;
}


