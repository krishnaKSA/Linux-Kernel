
1. Make sure that only one thread access shared data at a time.
2. Keep track of the reference. Reference counting helps.
   
We need to create critical section: code that can be executed by only one thread at a time.
For criticl section to work,  we must use a locking primitive that that works when a thread that owns the lock goes to sleep.

# Semaphore
Semaphore is single integer value with the pair of functions. A process wants to enter ctitical section, first it has invoke P fucntion of the corresponding 
semaphore. If the value of the semaphore is greater than 0, then value will be redcued by one, and process gets permission to access the data. In case, the value is zero or lesser than that
then it has wait until semaphore is released by someone else. 

```
#include <asm/semaphore.h>
void sema_init(struct semaphore *sem, int val); //Assign value as one for mutual exclusion

DECLARE_MUTEX(name); //initialized to 1
DECLARE_MUTEX_LOCKED(name); //Initialized to 0. It needs when we have start with closed.

//Dynamicaaly at runtime
void init_MUTEX(struct semaphore *sem);
void init_MUTEX_LOCKED(struct semaphore *sem);

void down(struct semaphore *sem); -> decrements the value and wait as long as to be

int down_interruptible(struct semaphore *sem); // same as above, but in this case, process is interruptible. But we need to check the return value to understand whether it holds semaphore at that time.

int down_trylock(struct semaphore *sem); -> this version doesn't sleep. If the process doen't get semaphore, it returns immediately

Once the thread gets semaphore, it access the crritical section. Once the work is done, it has to release the semaphore so other thread get chance to use it.

void up(struct semaphore* sem); -> increments the value by one.
```

# Reader and writer semaphore
semaphore provides mutual access. But sometime, many processes wants to read the data, when write is not happening. 
rw_semaphore aaccomplish it.

```
void init_rwsem(struct rw_semaphore *sem);
The interface for code needing read-only access is:

void down_read(struct rw_semaphore *sem);
int down_read_trylock(struct rw_semaphore *sem);
void up_read(struct rw_semaphore *sem);
The regular down_read could put you into an uninterruptable sleep, while the down_read_trylock version is interruptable. The rwsem obtained with down_read must be freed with up_read.

The interface for writers is similar:

void down_write(struct rw_semaphore *sem);
int down_write_trylock(struct rw_semaphore *sem);
void up_write(struct rw_semaphore *sem);
void downgrade_write(struct rw_semaphore *sem);

Again, write has an interruptable version called with the _trylock. The downgrade_write version is used if your writer needs to quickly change something, then needs read-only access for the rest of the time and can free the writing to another process.

Read/write semaphores allow one writer or an unlimited number of readers to hold the semaphore.
Writers get priority, and as soon as a writer enters the critical section, no readers are allowed until the writer has completed all work.
This can lead to reader starvation, where readers must wait a very long time to read. Thus, rwsems are best for minimal writes and lots of reading.
```

# Completion interface 

Completion interface is a lightweight mechanism with only one task. It will inform one thread to another that job is one. 

```
//Non interruptable process

struct completion my_completion;
/* ... */
init_completion(&my_completion);
Waiting for completion is only one line of code:

void wait_for_completion(struct completion *c);
Beware: this could create an uninterruptable process if completion never happens with the child process.

To signal completion, use:

void complete(struct completion *c);
void complete_all(struct completion *c);

complete -> informs only one thread. Complete all ->informs all the thread.
If complete_all is used then completion struct should be initialized before reusing it.
INIT_COMPLETION(struct completion*)
```
```
DECLARE_COMPLETION(comp);
ssize_t complete_read (struct file *filp, char __user *buf, size_t count, loff_t
*pos)
{
   printk(KERN_DEBUG "process %i (%s) going to sleep\n",
   current->pid, current->comm);
   wait_for_completion(&comp);
   printk(KERN_DEBUG "awoken %i (%s)\n", current->pid, current->comm);
   return 0; /* EOF */
}
ssize_t complete_write (struct file *filp, const char __user *buf, size_t count,
        loff_t *pos)
{
   printk(KERN_DEBUG "process %i (%s) awakening the readers...\n",
   current->pid, current->comm);
   complete(&comp);
   return count; /* succeed, to avoid retrial */
}
```

# Spin locks

Splin locks provides the mutual access. Its can't sleep like interrupt handlers.
Spin lock is a mutual exclusion device that can have only two values locked and unlocked.This is usually done with the single bit integer value. The code that wants to get the critical section , it tests the bit. If lock is available, this bit is set, and enters to critical section.If the lock is not available, then the code goes in to the tiny loop , and repeatedly check for the lock until it is available. This spin part is called spin lock. 

**spin lock api**
```
struct spinlock_t //defined in <linux/spinlock.h>
spinlock_t spinlock = SPIN_LOCK_UNLOCKED;
void spin_lock_init(spinlock_t* lock);
void spin_lock(spinlock_t* lock);
void spin_unlock(spinlock_t* lock);
Make sure that spinlock is uninterruptable. So it will be looping until lock is available.
```
Spinlock and atmoic context:

* any code that holds spin locks  should be atmoic. It can't sleep or relinquish processor for any resons expect few service interrupts.

*Kernel preemption is handled in the spinlock code, that means when the code holds spinlock, preemption is disabled in the releavnt processor.

Last rule: spinlocks should be held for the minimum amount of time possible. It can increase kernel latency and make other programs run slow.

Different spin lock API's
```
void spin_lock(spinlock_t *lock);
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags); //disables interrupts on the local processor before taking the spinlock. The previous interrupt state is stored in flags
void spin_lock_irq(spinlock_t *lock); //does not save the previous state. Only use if you are certain nothing else before you disabled interrupts.
void spin_lock_bh(spinlock_t *lock); //disables software interrupts before taking the lock but leaves hardware interrupts enabled

If the spinlock can be taken by code that runs in interrupt context, you must use one of the forms that disables interrupts to avoid deadlock. If it is only implemented inside of a software interrupt, use the spin_lock_bh to maintain the hardware interrupt features while being deadlock safe.

There are also four ways to release a spinlock which correspond to each of the locking functions:

void spin_unlock(spinlock_t *lock);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
void spin_unlock_irq(spinlock_t *lock);
void spin_unlock_bh(spinlock_t *lock);
The flags argument passed to spin_unlock_irqrestore must be the same variable passed to spin_lock_irqsave. You must also call spin_lock_irqsave and spin_ unlock_irqrestore in the same function to avoid breaking on some architectures.

There is also a set of nonblocking spinlock operations:

int spin_trylock(spinlock_t *lock);
int spin_trylock_bh(spinlock_t *lock);
These functions return nonzero on success, meaning the lock was obtained. They return 0 otherwise.
```

# Reader/Writer Spinlocks
The kernel provides a reader/writer set of spinlocks directly analogous to the reader/writer semaphores we saw earlier in the chapter. Again, lots of readers can all have access at once, but one writer must have exclusive access. R/W locks have a type rwlock_t defined in <linux/spinlock.h>. There are two ways to declare and initialize (statically and dynamically).
```
rwlock_t my_rwlock = RW_LOCK_UNLOCKED; /* Static way */

rwlock_t my_rwlock;
rwlock_init(&my_rwlock); /* Dynamic way */
The same variants of the four lock and unlock functions are available. Note there is no trylock for the reader. Not sure why...

void read_lock(rwlock_t *lock);
void read_lock_irqsave(rwlock_t *lock, unsigned long flags);
void read_lock_irq(rwlock_t *lock);
void read_lock_bh(rwlock_t *lock);

void read_unlock(rwlock_t *lock);
void read_unlock_irqrestore(rwlock_t *lock, unsigned long flags);
void read_unlock_irq(rwlock_t *lock);
void read_unlock_bh(rwlock_t *lock);
Write access has the familiar set of functions as well:

void write_lock(rwlock_t *lock);
void write_lock_irqsave(rwlock_t *lock, unsigned long flags);
void write_lock_irq(rwlock_t *lock);
void write_lock_bh(rwlock_t *lock);
int write_trylock(rwlock_t *lock);

void write_unlock(rwlock_t *lock);
void write_unlock_irqrestore(rwlock_t *lock, unsigned long flags);
void write_unlock_irq(rwlock_t *lock);
void write_unlock_bh(rwlock_t *lock);
Same as semaphores - don't starve the readers and don't have lengthy chuncks to write frequently. It will really slow down the system!
```
**Lock free mechanism**
Its good to have lock free algorthims when there is a possibility. 

1. Proceducer consumer program:
      In this producer consumer program, it has two index . one for read, one for write. This case is applicable when one writer and many readers. If both indexes pointing to the same location, that means no data to read. It doesn't create race condition. example <linux/kfifo.h>

2. Atomic variable:
      Sometimes shared value is simple integer value int number_pending_items. We may need to either increment or decrement  the value.
   Instead of using lock, we can use atomic functions. <asm/atomic.h>

```
      void atomic_set(atomic_t *v, int i);
      
      atomic_t v = ATOMIC_INIT(0); //set the integer value
      int atomic_read(atomic* v);
      void atomic_add(int i, atomic_t *v);
       //Add i to the atomic variable pointed to by v. The return value is void, because there is an extra cost to returning the new    
       value, and most of the time there’s no need to know it.
       
      void atomic_sub(int i, atomic_t *v);
       //Subtract i from *v.
       
      void atomic_inc(atomic_t *v);
      void atomic_dec(atomic_t *v);
       //Increment or decrement an atomic variable.
      
       int atomic_inc_and_test(atomic_t *v);
      int atomic_dec_and_test(atomic_t *v);
      int atomic_sub_and_test(int i, atomic_t *v);
       //Perform the specified operation and test the result; if, after the operation, the atomic value is 0, then the return value is 
       true; otherwise, it is false. Note that there is no atomic_add_and_test.
       
      int atomic_add_negative(int i, atomic_t *v);
       //Add the integer variable i to v. The return value is true if the result is negative, false otherwise.
       
      int atomic_add_return(int i, atomic_t *v);
      int atomic_sub_return(int i, atomic_t *v);
      int atomic_inc_return(atomic_t *v);
      int atomic_dec_return(atomic_t *v);
       //Behave just like atomic_add and friends, with the exception that they return the new value of the atomic variable to the caller.
      
      If there are multiple atmoic variable, then it has be properly handled with lock.
      
      atomic_sub(amount, &first_atomic);
      atomic_add(amount, &second_atomic);
```

# Bit operations

Atomic_t is good for interger values, but it doesn't support bit operations. Bit operations are supported in <asm/bitops.h> , its a single machine instruction not disabling interrupts. 

```
void set_bit(nr, void *addr);
    //Sets bit number nr in the data item pointed to by addr.
void clear_bit(nr, void *addr);
    //Clears the specified bit in the unsigned long datum that lives at addr. Its semantics are otherwise the same as set_bit.
void change_bit(nr, void *addr);
    //Toggles the bit.
test_bit(nr, void *addr);
    //This function is the only bit operation that doesn’t need to be atomic; it simply returns the current value of the bit.
int test_and_set_bit(nr, void *addr);
int test_and_clear_bit(nr, void *addr);
int test_and_change_bit(nr, void *addr);
    //Behave atomically like those listed previously, except that they also return the previous value of the bit.
```
```
/* try to set lock */
while (test_and_set_bit(nr, addr) != 0)
    wait_for_a_while( );
/* do your work */

/* release lock, and check... */
if (test_and_clear_bit(nr, addr) = = 0)
    something_went_wrong( ); /* already released: error */
```

# Seqlock

The old 2.6 kernel had some "new" mechanisms to provide fast, lockless access to a shared resource. Seqlocks work in situations where the resource to be protected is small, simple, and frequently accessed, and where write access is rare but must be fast. They work by allowing readers free access to the resource but requiring those same readers to check for collisions with writers. When this collision happens, the readers retry their access. Seqlocks generally cannot be used to protect data structures involving pointers because the reader may be following a pointer that is invalid while the writer is changing the data structure.
```
Seqlocks are defined in <linux/seqlock.h> and contain the two usual methods for initialization (static and dynamic). The type is seqlock_t.

seqlock_t lock1 = SEQLOCK_UNLOCKED;
seqlock_t lock2;

seqlock_init(&lock2);
The readers gain access by first obtaining an integer sequence value on entry into the critical section. On exit, that same sequence value is compared with the current value. If there is mismatch, the read access must be retried. Reader code looks like this:

unsigned int seq;
do {
    seq = read_seqbegin(&the_lock);
    /* Do what you need to do */
} while read_seqretry(&the_lock, seq);
We use this lock to protect some sort of simple computation that requires multiple, consistent values. If you need to access the seqlock from an interrupt handler, use the following IRQ-safe versions:

unsigned int read_seqbegin_irqsave(seqlock_t *lock,
    unsigned long flags);
int read_seqretry_irqrestore(seqlock_t *lock, unsigned int seq,
    unsigned long flags);
The exclusive writer lock can be obtained with:

void write_seqlock(seqlock_t *lock);
This acts like a typical spinlock. To release the spinlock, use:

void write_sequnlock(seqlock_t *lock);
The usual variants of the spinlock write functions are available for seqlock as well:

void write_seqlock_irqsave(seqlock_t *lock, unsigned long flags);
void write_seqlock_irq(seqlock_t *lock);
void write_seqlock_bh(seqlock_t *lock);
int write_tryseqlock(seqlock_t *lock);

void write_sequnlock_irqrestore(seqlock_t *lock, unsigned long flags);
void write_sequnlock_irq(seqlock_t *lock);
void write_sequnlock_bh(seqlock_t *lock);
```

# Read-Copy-Update (RCU)
RCU is an advanced mutual exclusion scheme (probably outdated by now) that can yield high performance under the proper conditions. It is rarely used in drivers but worth noting here.

RCU is optimized for situations where reads are common and writes are rare (that seems to be the case with most of these mechanisms). There are a few more constraints for this mechanism though:

Resources being protected should be accessed via pointers  All references to those resources must be held only by atomic code
When writing, the writing thread makes a copy, changes the copy, then aims the relevant pointer at the new version.
When the kernel is absolutely certain that no references to the old version remain, it can be freed
```
Include <linux/rcupdate.h> to use RCUs. Reader code will look something like this:

struct my_stuff *stuff;

rcu_read_lock( );
stuff = find_the_stuff(args...);
do_something_with(stuff);
rcu_read_unlock( );
rcu_read_lock is very fast. Code that executes while the read lock is held must be atomic, and no references to the protected resource can be used after the call to rcu_read_unlock.

Code that wants to change the protected structure needs to follow a few additional steps. It needs to:

Allocate a new structure

Copy data from the old one if needed
Replace the pointer used by read code
Now, any code entering the critical section sees the new version of the data. The last thing we need to do is free the old data. It can't be freed immediately because other readers may still be using a reference to older data. The code must wait until no more references exist. Since all references are atomic, once every process runs once on a processor, we can be sure that no more references exist. The RCU ends up setting aside a callback that cleans everything up once every process has been scheduled.

The writer must get its cleanup callback by allocating a new struct rcu_head. But no specific initialization is needed. After a change to the resource is made, a call should be made to:

void call_rcu(struct rcu_head *head, void (*func)(void *arg), void *arg);

The given func is called when it is safe to free the resource; it is passed to the same arg that was passed to call_rcu. Usually, func just calls kfree. The full RCU interface is more complex than this. If I ever need to use this I will probably need to do significantly more research on it.
```

