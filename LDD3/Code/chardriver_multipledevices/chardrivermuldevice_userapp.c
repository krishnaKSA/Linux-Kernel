#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    int fd_chdr1;
    int fd_chdr2;
    int err;
    
    char readbuffer[150];    
    char writebuffer[150];

    fd_chdr1= open("/dev/chdriver-1", O_RDWR);
    if(fd_chdr1<0)
    {
        printf("userapp: failed to open chdriver-1 node\n");
    }

    err = read(fd_chdr1, readbuffer, 24);
    if(err)
    {
        printf("userapp:failed to read\n");
    }
    else
    {
        printf("userapp: read successful count =%d \n", err);
    }
    
    err = write(fd_chdr1, writebuffer, 23);
    if(err < 0)
    {
        printf("userapp:failed to write\n");
    }
    else
    {
        printf("userapp: write successful \n");
    }

    fd_chdr2= open("/dev/chdriver-2", O_RDWR);
    if(fd_chdr2<0)
    {
        printf("userapp: failed to open chdriver-2 node\n");
    }

    err = read(fd_chdr2, readbuffer, 45);
    if(err<0)
    {
        printf("userapp:failed to read\n");
    }
    else
    {
        printf("userapp: read successful =%d\n", err);
    }
    
    err = write(fd_chdr2, writebuffer, 23);
    if(err < 0)
    {
        printf("userapp:failed to write\n");
    }
    else
    {
        printf("userapp: write successful count=%d\n", err);
    }

    close(fd_chdr2);

    close(fd_chdr1);
    return 0;
}