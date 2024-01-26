
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


```
