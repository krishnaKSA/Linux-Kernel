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



