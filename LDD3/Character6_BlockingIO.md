# Blocking IO
Sometime when we request for read or write to IO device, it can't be executed on that time beacuse IO is busy. In this case, we put the process to sleep and wake up when it is available to respons.

When the process moves to sleep, its in special state, it will be removed from the schdulers run queue , and it will come out of the sleep state until someone change its state.

- Rule 1: Never sleep if you are running in the atomic context, don't go to sleep when holding spin lock, seq lock , rcu lock, and while having disabled interrupts.
+ Rule 2: When you wake up, you never know how long you were out for or what has changed (like a rough Friday night). Make no assumptions about the state of the system when you wake up. Check every condition you were waiting for is actually true.
- Rule 3: You can't go to sleep unless somebody else is capable of waking you up. This is accomplished with a data structure called a wait queue. It is a list of processes all waiting for a specific event.

In Linux, a wait queue is managed by means of a wait queue head, which is a structure of type wait_queue_head_t defined in <linux/wait.h>. It can be allocated statically or dynamically with:

```
DECLARE_WAIT_QUEUE_HEAD(name); //static init

wait_queue_head_t my_queue; //dynamic init
init_waitqueue_head(&my_queue);
```
# simple sleep

Process will move to wait state until some condition to occur, When it comes out of sleep, it has to make sure that condition has occurred.
Its handled by **wait_event**.

```
wait_event(queue, condition) ->queue is wait_queue_head_t
wait_event_interruptible(queue, condition)
wait_event_timeout(queue, condition, timeout) -> These two versions are timeopit based
wait_event_interruptible_timeout(queue, condition, timeout)
```
```
void wake_up(wait_queue_head_t *queue);
void wake_up_interruptible(wait_queue_head_t *queue); -> this only wakeup the process in the wait queue which has interrupt enabled
```

Example
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
Since wait_event_interruptible checks for a condition to be true, the flag variable creates that condition. Note there is a possible race condition that opens up two sleeping processes at the same time and they both read the flag as nonzero before they reset it. This operation would need to be done in an atomic matter - we will get to that shortly.
```

# Blocking and non blocking operations
There are some cases where process doesn't want to block even if the requested IO opeation didn't get succeeded. In this case process set O_NONBLOCK flag while opening the device node. Its present in filp->f_flags.

1. If a process calls read but data is not yet available, the process must block. The process is awakened as soon as data arrives.
2. If a process calls write and the buffer has no space, the process must block until room is available on the buffer. This could result in partial writes if count is larger than the free space in the buffer.

Almost every driver has input and output buffers. Input prevents loss of data, and output speeds up performance of the system. Having output buffers reduces context switching and user/kernel transistions. The really help speed up performance!

If the O_NONBLOCk flag is set, driver returns -EAGAIN if the reader doesn't have any data to read or write doesn't have space to store the data.
Overall - only the read, write, and open file operations are affected by the nonblocking flag.

**Example**
```
process blocked in a read call is awakened when data arrives (usually handled as an interrupt). Scullpipe does not use an interrupt handler, however. The device uses two queues and a buffer. The buffer size in configurable in the usual ways.

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

# Avdvanced sleeping

If you look in <linux/wait.h> there will be a data structure for wait_queue_head_t that is pretty simple. There is a spinlock and a linked list. The list contains a wait queue entry declared with type wait_queue_t.

The first step in going to sleep is the allocation and initialization of the wait_queue_t structure, followed by its addition to the proper wait queue. That way the person in charge of wakeup can find the right process.

The next step is to set the task in a state of "asleep". In <linux/sched.h> there are several task states defined. Some of them are:

TASK_RUNNING - a process is able to run
TASK_INTERRUPTIBLE - interruptible sleep (use this one)
TASK_UNINTERRUPTIBLE - uninterruptible sleep
You should not directly change the state of a process, but if you need to, use the following:

void set_current_state(int new_state);
Now that we have changed our state, we need to formally give up the processor. First, check to make sure that your wakeup condition is not true already, which could create a race condition if you were to check this before setting your state to sleep. Some code like this should exist after setting the process state:

if (!condition)
    schedule( );
Also make sure that the state is changed again after waking from sleep!

Manual sleep can still be done. First create a wait queue entry with:

DEFINE_WAIT(my_wait); (preferred)

//OR in 2 steps:
wait_queue_t my_wait;
init_wait(&my_wait);
Next, add your wait queue entry to the queue and set the process state. Both are handled with:

void prepare_to_wait(wait_queue_head_t *queue, wait_queue_t *wait, int state);
queue and wait are the queue head and process entry
state is the new state for the process (usually TASK_INTERRUPTIBLE)
After calling this function, the process can call schedule() after it has checked that it still needs to wait. Once schedule returns, it is cleanup time. That is handled with:

void finish_wait(wait_queue_head_t *queue, wait_queue_t *wait);
Now let's look at the write method for scullpipe:
```
/* How much space is free? */
static int spacefree(struct scull_pipe *dev)
{
  if (dev->rp = = dev->wp)
    return dev->buffersize - 1;
  return ((dev->rp + dev->buffersize - dev->wp) % dev->buffersize) - 1;
}
static ssize_t scull_p_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  struct scull_pipe *dev = filp->private_data;
  int result;
  if (down_interruptible(&dev->sem))
    return -ERESTARTSYS;   
 
 /* Make sure there's space to write */
  result = scull_getwritespace(dev, filp);
  if (result)
    return result; /* scull_getwritespace called up(&dev->sem) */
 
 /* ok, space is there, accept something */
  count = min(count, (size_t)spacefree(dev));
  if (dev->wp >= dev->rp)
    count = min(count, (size_t)(dev->end - dev->wp)); /* to end-of-buf */
  else /* the write pointer has wrapped, fill up to rp-1 */
    count = min(count, (size_t)(dev->rp - dev->wp - 1));
 PDEBUG("Going to accept %li bytes to %p from %p\n", (long)count, dev->wp, buf);
 if (copy_from_user(dev->wp, buf, count)) {
  up (&dev->sem);
  return -EFAULT;
 }
 dev->wp += count;
 if (dev->wp = = dev->end)
    dev->wp = dev->buffer; /* wrapped */
 up(&dev->sem);
 
 /* finally, awake any reader */
 wake_up_interruptible(&dev->inq); /* blocked in read( ) and select( ) */
 
 /* and signal asynchronous readers, explained late in chapter 5 */
 if (dev->async_queue)
 kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
 PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)count);
 return count;
}
And the code that handles sleeping is:

/* Wait for space for writing; caller must hold device semaphore. On
 * error the semaphore will be released before returning. */
static int scull_getwritespace(struct scull_pipe *dev, struct file *filp)
{
   while (spacefree(dev) = = 0) { /* full */
      DEFINE_WAIT(wait);
      up(&dev->sem);
      if (filp->f_flags & O_NONBLOCK)
          return -EAGAIN;
      PDEBUG("\"%s\" writing: going to sleep\n",current->comm);
      prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
      if (spacefree(dev) = = 0)
          schedule( );
      finish_wait(&dev->outq, &wait);
      if (signal_pending(current))
          return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
      if (down_interruptible(&dev->sem))
          return -ERESTARTSYS;
   }
   return 0;
}
```
In this code, if space is available without sleeping, the function just returns. Otherwise, it needs to drop the device semaphore and wait. It uses DEFINE_WAIT to set up a wait queue entry and prepare_to_wait to get ready for the actual sleep. Then after a quick on the buffer to make sure the wake signal hasn't already been triggered, we can call schedule().

In the case where only one process in the queue should awaken instead of all of them, we need a way to handle exclusive waits. It acts like a normal sleep with two big differences:

When a wait queue entry has the WQ_FLAG_EXCLUSIVE flag set, it is added to the end of the wait queue. Entries without that flag are, instead, added to the beginning.
When wake_up is called on a wait queue, it stops after waking the first process that has the WQ_FLAG_EXCLUSIVE flag set.
The kernel will still wakeup all nonexclusive waiters every time, but with this method an exclusive waiters will not have to create a "thundering herd" of processes waiting their turn still.

You might use exclusive waits if:

you expect significant contention for a resource AND
waking a single process is sufficient to completely consume the resource when it becomes available
This is used for Apache web servers. Putting a process into interruptable wait only requires changing one line:

void prepare_to_wait_exclusive(wait_queue_head_t *queue, wait_queue_t *wait, int state);
Now we need to wake up. Here are all the variants to use:

wake_up(wait_queue_head_t *queue);
wake_up_interruptible(wait_queue_head_t *queue);
  /*wake_up awakens every process on the queue that is not in an exclusive wait,
  and exactly one exclusive waiter, if it exists. wake_up_interruptible does the
  same, with the exception that it skips over processes in an uninterruptible sleep.
  These functions can, before returning, cause one or more of the processes awakened 
  to be scheduled (although this does not happen if they are called from an
  atomic context).*/
  
wake_up_nr(wait_queue_head_t *queue, int nr);
wake_up_interruptible_nr(wait_queue_head_t *queue, int nr);
  /*These functions perform similarly to wake_up, except they can awaken up to nr
  exclusive waiters, instead of just one. Note that passing 0 is interpreted as 
  asking for all of the exclusive waiters to be awakened, rather than none of 
  them.*/
  
wake_up_all(wait_queue_head_t *queue);
wake_up_interruptible_all(wait_queue_head_t *queue);
  /*This form of wake_up awakens all processes whether they are performing an
  exclusive wait or not (though the interruptible form still skips processes doing
  uninterruptible waits).*/
  
wake_up_interruptible_sync(wait_queue_head_t *queue);
  /*Normally, a process that is awakened may preempt the current process and be
  scheduled into the processor before wake_up returns. In other words, a call to
  wake_up may not be atomic. If the process calling wake_up is running in an
  atomic context (it holds a spinlock, for example, or is an interrupt handler), 
  this rescheduling does not happen. Normally, that protection is adequate. If, 
  however, you need to explicitly ask to not be scheduled out of the processor at 
  this time, you can use the “sync” variant of wake_up_interruptible. This function 
  is most often used when the caller is about to reschedule anyway, and it is more
  efficient to simply finish what little work remains first.*/
Most of the time, drivers just use wake_up_interruptible and not any other special variants.

