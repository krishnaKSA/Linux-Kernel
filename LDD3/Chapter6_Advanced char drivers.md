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
```

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

# Blocking I/O

If the process can't write/read from the device , since device is busy. In this case, we can block the process and put in sleep until the resquest can be processes.

when the process goes to the sleep mode, it removed from the process queue, and it moved to wait queue.

Rule 1: Never go to sleep in automatic context. We shouldn't move to sleep state in spinlock, rcu lock, seq lock, and if we have disabled interrupts.

Rule2: After wake up, again we need to check necessary system state check, beacuse somethins might have changed when process was in sleep state.

Rule3: Don't move to sleep state without having proper wakup method.

In Linux, a wait queue is managed by means of a wait queue head, which is a structure of type wait_queue_head_t defined in <linux/wait.h>. It can be allocated statically or dynamically with:
```
DECLARE_WAIT_QUEUE_HEAD(name); //static init

wait_queue_head_t my_queue; //dynamic init
init_waitqueue_head(&my_queue);
```

```
wait_event(queue, condition)
wait_event_interruptible(queue, condition)
wait_event_timeout(queue, condition, timeout)
wait_event_interruptible_timeout(queue, condition, timeout)
```

*queue is the wait queue head to use

*condition is an arbitrary boolean condition evaluated by the macro before AND after sleeping

*The interruptible version can be interrupted by signals and is the preferred method to use. This version returns and INT that will be nonzero if a signal interrupted it, and you should return a -ERESTARTSYS in that case.

*The final two versions have a timeout measured in jiffies that will only wait for a certain period of time before returning a zero.

The other half is waking up from sleep. Another thread actually has to wake you up, because you are asleep and won't respond to normal things. The basic function is wake_up and two forms are given below:

```
void wake_up(wait_queue_head_t *queue);
void wake_up_interruptible(wait_queue_head_t *queue);
```

This will wake up all processes on the current queue. Usually, you use the interruptible version for going to sleep with interrupts enabled, since this version will only wake up queue items that have interrupts.


```
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;
ssize_t sleepy_read (struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
   printk(KERN_DEBUG "process %i (%s) going to sleep\n",
      current->pid, current->comm);
   wait_event_interruptible(wq, flag != 0);
   flag = 0;
   printk(KERN_DEBUG "awoken %i (%s)\n", current->pid, current->comm);
   return 0; /* EOF */
}
  ssize_t sleepy_write (struct file *filp, const char __user *buf, size_t count,
      loff_t *pos)
{
   printk(KERN_DEBUG "process %i (%s) awakening the readers...\n",
      current->pid, current->comm);
   flag = 1;
   wake_up_interruptible(&wq);
   return count; /* succeed, to avoid retrial */
}
```

# Blocking and unblocking operations

When do we put a process to sleep? There are in fact times when you want an operation not to block, even if it cannot be completely carried out. There are also times when the calling process informs you that it does not want to block, whether or not its I/O can make any progress. Explicit nonblocking I/O is indicated by the O_NONBLOCK flag in flip->f_flags. This flag is defined in <linux/fcntl.h> included in <linux/fs.h>. The name of O_NONBLOCK is derived from "open-nonblock" because it can be specified at open time. How behavior of a blocking operation should be implemented:

If a process calls read but data is not yet available, the process must block. The process is awakened as soon as data arrives.
If a process calls write and the buffer has no space, the process must block until room is available on the buffer. This could result in partial writes if count is larger than the free space in the buffer.
Almost every driver has input and output buffers. Input prevents loss of data, and output speeds up performance of the system. Having output buffers reduces context switching and user/kernel transistions. The really help speed up performance!

Scull does not use input and output buffers because all data is available and simply copied to the correct location. Chapter 10 will get more into buffers (to get buff).

The behavior of read and write is different if O_NONBLOCK is specified. The calls return -EAGAIN if a process calls read when no data is available or if it calls write when there's no space left in the buffer. Nonblocking operations return immediately, allowing the application to poll for data. Be careful using stdio while dealing with nonblocking files - do more research on this if you are going to implement it.

O_NONBLOCK is important for the open method if you have a long initialization process. It can return -EAGAIN after starting the init process to make sure everything is setup properly.

Overall - only the read, write, and open file operations are affected by the nonblocking flag.

```
A Blocking I/O Example
We will look at the scullpipe driver as an implementation of a blocking I/O.

A process blocked in a read call is awakened when data arrives (usually handled as an interrupt). Scullpipe does not use an interrupt handler, however. The device uses two queues and a buffer. The buffer size in configurable in the usual ways.

struct scull_pipe {
   wait_queue_head_t inq, outq; /* read and write queues */
   char *buffer, *end; /* begin of buf, end of buf */
   int buffersize; /* used in pointer arithmetic */
   char *rp, *wp; /* where to read, where to write */
   int nreaders, nwriters; /* number of openings for r/w */
   struct fasync_struct *async_queue; /* asynchronous readers */
   struct semaphore sem; /* mutual exclusion semaphore */
   struct cdev cdev; /* Char device structure */
};
read manages both blocking and nonblocking as the following:

static ssize_t scull_p_read (struct file *filp, char __user *buf, size_t count,
    loff_t *f_pos)
{
    struct scull_pipe *dev = filp->private_data;
    
    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
 while (dev->rp = = dev->wp) { /* nothing to read */
    up(&dev->sem); /* release the lock */
    
    if (filp->f_flags & O_NONBLOCK)
        return -EAGAIN;
    PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
    
    if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)))
        return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
        
    /* otherwise loop, but first reacquire the lock */
    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    }
    
  /* ok, data is there, return something */
  if (dev->wp > dev->rp)
      count = min(count, (size_t)(dev->wp - dev->rp));
  else /* the write pointer has wrapped, return data up to dev->end */
      count = min(count, (size_t)(dev->end - dev->rp));
  if (copy_to_user(buf, dev->rp, count)) {
      up (&dev->sem);
      return -EFAULT;
  }
  dev->rp += count;
  if (dev->rp = = dev->end)
      dev->rp = dev->buffer; /* wrapped */
  up (&dev->sem);
  /* finally, awake any writers and return */
  wake_up_interruptible(&dev->outq);
  PDEBUG("\"%s\" did read %li bytes\n",current->comm, (long)count);
  return count;
}
Some PDEBUG messages are left in there - you can enable/disable them at compile time for the driver with a flag.

Let's look at how scull_p_read handles waiting for the data:

The while loop tests the buffer with the device semaphore held
If the data is there, there is no need to sleep and data can be transferred to the user
If the buffer is empty, we must sleep
Before sleeping, we NEED to drop the device semaphore
When the semaphore is dropped, we make a quick check to see if the user has requested non-blocking I/O and return if true. If not, time to call wait_event_interruptible.
Now we are asleep, and need some way to wake up. One way is if the process receives a signal - the if statement contains a wait_event_interruptible call to check for this case.
When awoken, you must aquire the device semaphore again (somebody else could have read the data too after waiting like you did!) and check that there is data to be read.
Thus, when we exit the while loop, we know the semaphore is held and there is data to be read
```



