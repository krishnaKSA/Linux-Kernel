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
#define CHDRIVER_MAGICNUMBER 'k'
#define CHDRIVER_IOCGETDATA _IOR(k,1,int) //READ int data type
#define CHDRIVER_IOCSETDATA _IOW(k,2,int) //Write int data
#define CHDRIVER_MAX_NR 3

Here, k is a magaic number. Used throughout. 1, 2 are numbers in sequential order. int is the data type.


# IOCTL argument

If the cmd argument contains pointer, we need to make sure whether the address of the user space is ok before proceeding further.

```
int access_ok(int type, const void *addr, unsigned long size);
```

If it is a read operation, then we need to check "VERIFY_WRITE".
If it is a write operation, then we need to check "VERIFY_READ".

```
int ret = 0;

//check magic number
if(_IOC_TYPE(cmd) != CHDRIVER_MAGICNUMBER)
	return -ENOTTY; //invalid cmd
if(_IOC_NR(cmd) != CHDRIVER_MAX_NR)
	return -ENOTTY;

if(_IOC_DIR(cmd) & _IOC_READ)
	ret = access_ok(VERIFY_WRITE, (void __user*)arg,_IOC_SIZE(cmd));
else if(_IOC_DIR(cmd) & _IOC_WRITE)
	ret = access_ok(VERIFY_READ, (void __user*)arg,_IOC_SIZE(cmd));
if(ret) return -EFAULT;
```

After permforming access check, device can safety transfer the data. Apart from copy_from_user, copy_to_user , there are other optimised options are there.

```
put_user(datum, ptr)
__put_user(datum, ptr)
  These macros write the datum to user space; they are relatively fast and should be called instead of copy_to_user whenever single values are being transferred. The macros have been written to allow the passing of any type of pointer to put_user, as long as it is a user-space address. The size of the data transfer depends on the type of the ptr argument and is determined at compile time using the sizeof and typeof compiler builtins. As a result, if ptr is a char pointer, one byte is transferred, and so on for two, four, and possibly eight bytes. 
  put_user checks to ensure that the process is able to write to the given memory address. It returns 0 on success, and -EFAULT on error. __put_user performs less checking (it does not call access_ok), but can still fail if the memory pointed to is not writable by the user. Thus, __put_user should only be used if the memory region has already been verified with access_ok. 
  As a general rule, you call __put_user to save a few cycles when you are implementing a read method, or when you copy several items and, thus, call access_ok just once before the first data transfer, as shown above for ioctl.

get_user(local, ptr)
__get_user(local, ptr)
  These macros are used to retrieve a single datum from user space. They behave like put_user and __put_user, but transfer data in the opposite direction. The value retrieved is stored in the local variable local; the return value indicates whether the operation succeeded. Again, __get_user should only be used if the address has already been verified with access_ok.

```

# Capablities and restriction operations

Access to a device is controlled by the permissions on the device files, and the driver itself is normally not involved with this permissions checking. There are situations where any user has read/write permission on the device, but some control operations should still be denied. Thus, the driver needs to perform some additional checks to make sure the user is capable of performing the requested operation.

Unix systems traditionally used two users - the normal user and the superuser. The normal user is highly restricted, and the superuser can do anything. Sometimes we need a solution inbetween the two. This is why the kernel provides a more flexible system call capabilities. This system breaks down privileged operations into separate subgroups. The kernel exports two system calls capget and capset to allow permissions to be managed from user space. The full set of capabilities is in <linux/capability.h>. The subset of capabilities were are interested in:
```
CAP_DAC_OVERRIDE
  The ability to override access restrictions (data access control, or DAC) on files and directories.
  
CAP_NET_ADMIN
  The ability to perform network administration tasks, including those that affect network interfaces.
  
CAP_SYS_MODULE
  The ability to load or remove kernel modules.
  
CAP_SYS_RAWIO
  The ability to perform “raw” I/O operations. Examples include accessing device ports or communicating directly with USB devices.
  
CAP_SYS_ADMIN
  A catch-all capability that provides access to many system administration operations.
  
CAP_SYS_TTY_CONFIG
The ability to perform tty configuration tasks.

```
Before performing a privileged operation, a device driver should check to make sure the calling process has the appropriate capability. Capability checks are performed with the capable function found in <linux/sched.h>:
```
int capable(int capability);
```
```
if (! capable (CAP_SYS_ADMIN))
  return -EPERM;
```




