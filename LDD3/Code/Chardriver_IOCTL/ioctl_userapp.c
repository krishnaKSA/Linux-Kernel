#include <stdio.h>
#include "fcntl.h" //ORDWR
#include "unistd.h" //open close
#include <sys/ioctl.h> //_IOW _IOR

#define IOCTLWRITE _IOW('a', 1, unsigned int* )
#define IOCTLREAD _IOR('a', 2, unsigned int*)

int main()
{
    //open file
    int fd;
    int ret;
    unsigned int baudrate=0;

    fd = open("/dev/ioctldriver-0", O_RDWR);
    if(fd < 0)
    {
        printf("IOCTL:USRAPP: Open failed\n");
    }

    if(ioctl(fd, IOCTLREAD,(unsigned int*) &baudrate)<0)
    {
        printf("IOCTL:USRAPP: read failed \n");
    }
    else{
        printf("IOCTL:USRAPP: read completed . baudrate = %d \n", baudrate);
    }

    baudrate = 500;
    if(ioctl(fd, IOCTLWRITE,(unsigned int*)  &baudrate)<0)
    {
        printf("IOCTL:USRAPP: write failed \n");
    }
    else{
        printf("IOCTL:USRAPP: write completed . baudrate = %d \n", baudrate);
    }

    close(fd);
    return 0;
}