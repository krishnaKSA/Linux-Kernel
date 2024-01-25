# IOCTL
Apart from doing read, write opeartions, sometimes it required to control hardware via drivers.

In user space, ioctl protocol is below
```
int ioctl(int fd, unsigned long cmd, ...);
```

Kernel space ioctl prototype
```
int (*ioctl) (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
```
* inode , flip is corresponding to the file descriptor "fd" passed by the user.
* cmd is passed by the user
* arg is passed by the user regardless of how user gave it, its unsigned long.

**Choosing ioctl command number**
Methods to choose ioctl command numbers defined in include/asm/ioctl.h and Documentation/ioctl-number.txt

Approved way to define ioctl command numbers defined in four bit fields. 
```
#define _IOC(dir,type,nr,size) \
	(((dir)  << _IOC_DIRSHIFT) | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT) | \
	 ((size) << _IOC_SIZESHIFT))
```
Type:   The magic number. Just choose a new number not in ioctl-number.txt and use it throughout the driver. This field is 8 bits wide.

Number: The sequential number. It is 8 bits wide.

Direction: The direction of data transfer. Possible values include:
_IOC_NONE (no data transfer)
_IOC_READ
_IOC_WRITE
_IOC_READ|_IOC_WRITE (both read and write)
This transfer is seen from the application's point of view (read means read from the device)

Size :The size of user data involved. The width of this field is architechure dependent, but usually 13 or 14 bits. You can find it with the macro _IOC_SIZEBITS. This field is not mandatory, and the kernel doesn't actually check for it, but it is good practice for debugging.

```
If you are adding new ioctl's to the kernel, you should use the _IO
macros defined in <linux/ioctl.h>:

    _IO    an ioctl with no parameters
    _IOW   an ioctl with write parameters (copy_from_user)
    _IOR   an ioctl with read parameters  (copy_to_user)
    _IOWR  an ioctl with both write and read parameters.
```

```
The header file <asm/ioctl.h> included by <linux/ioctl.h> defines macros that help set up command numbers as follows:

_IO(type, nr) for a command with no argument
_IOR(type,nr,datatype) for reading data from the driver
_IOW(type,nr,datatype) for writing data
_IOWR(type,nr,datatype) for bidirectional transfers
```

```
For example, 'k' is the magic number.
#define CHDRIVER_IOCGETDATA _IOR(k,1,int) //READ int data type
#define CHDRIVER_IOCSETDATA _IOW(k,2,int) //Write int data

Here, k is a magaic number. Used throughout. 1, 2 are numbers in sequential order. int is the data type.

