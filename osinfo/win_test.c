
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
    int fd = 0;
    fprintf(stderr, "Opening %s \n", regfilename);
<<<<<<< Updated upstream
    return open(regfilename, mod);
}

int myread(int fd, char *buf, int size)
=======
    //dumpfd = open("/tmp/dump1", O_CREAT|O_WRONLY|O_TRUNC, 077);
    fd = open(regfilename, mod);
    if(fd<0)
        perror("Failed to open\n");
    printf("Open : %d\n", fd);
    return fd;
}

int myread(int fd, char *buf, int size)
{
    int ret =0;
    ret = read(fd, buf, size);
    if(ret <0)
        perror("Failed to read ....\n");
    readcount += ret;
    if(dumpfd) {
        write(dumpfd, buf, ret);
    }
    //fprintf(stderr, "Read %d asked %d \n", ret, size);

    return ret;
}
int mypread(int fd, char *buf, int size, size_t off)
>>>>>>> Stashed changes
{
    int ret =0;
    ret = read(fd, buf, size);
    if(ret <0)
        perror("Failed to read ....\n");
<<<<<<< Updated upstream
    fprintf(stderr, "Read %d asked %d \n", ret, size);
=======
    readcount += ret;
    if(dumpfd) {
        pwrite(dumpfd, buf, ret, off);
    }
    //fprintf(stderr, "Read %d asked %d \n", ret, size);
>>>>>>> Stashed changes

    return ret;
}
int mylseek(int fd, off_t off, int wh)
{
    int ret =0;
    ret = lseek(fd, off, wh);
    if(ret <0)
        perror("Failed to read ....\n");
<<<<<<< Updated upstream
    fprintf(stderr, "lseek from %d off %d \n", wh, (int)off);
=======
    //fprintf(stderr, "Seek off %d wh %d \n", (int)off, wh);
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
    osi_get_os_details(myopen, myread, mylseek, &info);     
=======
    osi_get_os_details("windows", myopen, NULL, myread, mylseek, &info);     
>>>>>>> Stashed changes
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


