# **PROCESS :**
  * Process is more than the progranning code, which is sometimes knowns as the text section.
  * Process includes current activity, as represented by the value of **Program counter**, and contents of the processor registers.
  * A process generally includes the process **stack** , which contains temporary data variable, function arguments, return address),
    and a data section which includes global variable.
  * A process also includes heap , which is memory that is dynamically allocated during the process run time.

A program is a passive entity, such as a file containing a list of instructions stored on disk
(often called an executable file). In contrast, a process is an active entity, with a program counter specifying the next instruction to execute and a set of associated resources. A program becomes a process when an executable file is loaded into memory.

**The structure of the process is below.**
 
 ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/517c5326-a11f-45ae-b330-bd91aa487685)


![image(1620)](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/230cf423-078d-4dcb-97a7-e8004eabe82d)

# **Process state:**
As a process executes, it changes state. The state of a process is defined in part by the current activity of that process. 
A process may be in one of the following states:
• New. The process is being created.
• Running. Instructions are being executed.
• Waiting. The process is waiting for some event to occur (such as an I/O
completion or reception of a signal).
• Ready. The process is waiting to be assigned to a processor.
• Terminated. The process has finished execution

# **Process Control Block**
Each process is represented in the operating system by a process control block (PCB)—also called a task control block.

![Process-state-transition-diagram](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/75e44a17-5816-4e4b-83de-16964a2f346a)

• **Process state.** The state may be new, ready, running, waiting, halted, and so on.

• **Program counter.** The counter indicates the address of the next instruction to be executed for this process.

• **CPU registers.** The registers vary in number and type, depending on the computer architecture. They include accumulators, index registers, stack pointers, and general-purpose registers, plus any condition-code  information. Along with the program counter, this state information must be saved when an interrupt occurs, to allow the process to be continued
correctly afterward.

• **CPU-scheduling information.** This information includes a process priority, pointers to scheduling queues, and any other scheduling parameters.

• **Memory-management information.** This information may include such items as the value of the base and limit registers and the page tables, or the segment tables, depending on the memory system used by the operating system.

# **PCB Control block**

![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/0d1359b7-8fee-4e8a-9427-1cac0cec4cc1)

# **CPU switch from one process to another**

![R](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/b98a982a-d4e6-425e-9afa-01be5af8f042)

# **Process representation in linux**

![3_05_LinuxProcesses](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/9814fbb2-e361-435e-95a1-c63225e6da1a)

# PROCESS SCHEDULING

The objective of the multiprogramming is to have some process running at all times, to maximize the CPU uitlization.  The objective of time sharing is to switch the CPU among processes so frequently that users can interact with each program white it is running. To meet , the **Program scheduler** selects an available process for program execution on the CPU.

![R](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/3db6d34f-54e4-4d83-9554-a7e80c10bc27)

# SCHEDULING QUEUES

**Job queue** ->  contains all the processes in the system.

**Ready queue**  -> contains all the processes which is in ready or waiting state.
These queues are generally linked lists.

**Device queue** -> Each device contains its own device queue. It contains the list of processes which are waiting for the particular I/O device.

![R](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/d0e2c291-ccb1-4705-b5a1-d0d76a38e6e9)

# SCHEDULERS
* A **long-term scheduler** is typical of a batch system or a very heavily loaded system. It runs infrequently, ( such as when one process ends selecting one more to be loaded in from disk in its place ), and can afford to take the time to implement intelligent and advanced scheduling algorithms.

* The **short-term scheduler, or CPU Scheduler**, runs very frequently, on the order of 100 milliseconds, and must very quickly swap one process out of the CPU and swap in another one.

* Some systems also employ a** medium-term scheduler**. When system loads get high, this scheduler will swap one or more processes out of the ready queue system for a few seconds, in order to allow smaller faster jobs to finish up quickly and clear the system. 

* An efficient scheduling system will select a good **process mix of CPU-bound processes and I/O bound processes.**
  
![3_07_QueuingDiagram2](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/3e6f588b-00b6-4bff-928f-737fb7cf4d74)

# CONTEXT SWITCHING

*Whenever an interrupt arrives, the CPU must do a state-save of the currently running process, then switch into kernel mode to handle the interrupt, and then do a state-restore of the interrupted process.

*Similarly, a context switch occurs when the time slice for one process has expired and a new process is to be loaded from the ready queue. This will be instigated by a timer interrupt, which will then cause the current process's state to be saved and the new process's state to be restored.

*Saving and restoring states involves saving and restoring all of the registers and program counter(s), as well as the process control blocks described above.

*Context switching happens VERY VERY frequently, and the overhead of doing the switching is just lost CPU time, so context switches ( state saves & restores ) need to be as fast as possible. Some hardware has special provisions for speeding this up, such as a single machine instruction for saving or restoring all registers at once.

*Some Sun hardware actually has multiple sets of registers, so the context switching can be speeded up by merely switching which set of registers are currently in use. Obviously there is a limit as to how many processes can be switched between in this manner, making it attractive to implement the medium-term scheduler to swap some processes out.

![3_MultitaskingMobileSystems](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/3a4920dc-97b8-418c-a711-0e286a74b0ba)

# PROCESS CREATION

* Process may create other process by invoking **fork** or **spawn**.
  
* PPID - Parent process ID.

* The first thing it does at system startup time is to launch **init** , which gives process ID 1. Init launches all system deamons , user login , and becomes ultimate parent for all the processes.

* child process will need certain resources like CPU time, memory files, IO device. A child process may get this directly from OS or parent process does parition its resources among its childrens or it shares some resources.

* In addition to supplying physical and logical resources, sometimes it gives initialization data(input).
  For example, the job of the process is to display the content of the file like image.jpg. When the child process is created , it gives name of the image files to be displayed and it gives the outout device as well.

**When a process creates a new process, two possibilities for execution exist:**
1. The parent continues to execute concurrently with its children.

2. The parent waits until some or all of its children have terminated

**There are also two address-space possibilities for the new process:**
1. The child process is a duplicate of the parent process (it has the same program and data as the parent).

2. The child process has a new program loaded into it

In fork(), child process consists of a copy of the address space of a original process.
Child proces inherits privilages and scheduling attributes , and some resources as open files.

![3_08_ProcessTree](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/e35f6734-a8ff-46a6-aade-a73074642c61)

![3_10_ProcessCreation](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/a53f585f-2166-4c6c-a802-aa83ff90a2a3)

```
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
int main()
{
pid t pid;
/* fork a child process */
pid = fork();
if (pid < 0) { /* error occurred */
fprintf(stderr, "Fork Failed");
return 1;
}
else if (pid == 0) { /* child process */
execlp("/bin/ls","ls",NULL);
}
else { /* parent process */
/* parent will wait for the child to complete */
wait(NULL);
printf("Child Complete");
}
return 0;
}
```

# PROCESS TERMINATION

 * Process may request own termination call by invoking **exit** function whicn return int value. If   
   success m then zero returns, on unsuccessful cases non zero value returned. This int return value will 
   be passed to parent function if it is in **wait()** state.

           ```
            child code:
            int exitCode;
            exit( exitCode );  // return exitCode; has the same effect when executed from main( )

            parent code:
            pid_t pid;
            int status
            pid = wait( &status ); 
            // pid indicates which child exited. exitCode in low-order bits of status
            // macros can test the high-order bits of status for why it stopped
          ```

 * Process can also be terminated on the below reasons,****

   ->The inability of the system to deliver necessary system resources.

   -> In response to a KILL command, or other un handled process interrupt.

   -> A parent may kill its children if the task assigned to them is no longer needed.

   ->If the parent exits, the system may or may not allow the child to continue without a parent. ( On UNIX 
     systems, orphaned processes are generally inherited by init, which then proceeds to kill them. The 
     UNIX nohup command allows a child to continue executing after its parent has exited. )

* When a process terminates, all of its system resources are freed up, open files flushed and closed, etc. The process termination status and execution times are returned to the parent if the parent is waiting for the child to terminate, or eventually returned to init if the process becomes an orphan. ( Processes which are trying to terminate but which cannot because their parent is not waiting for them are termed zombies. These are eventually inherited by init as orphans and killed off. Note that modern UNIX shells do not produce as many orphans and zombies as older systems used to. )


# INTERPROCESS COMMUNICATION

Independent process which is not sharing any data or need any info from otjer processes. Cooperating process which can affect or is affected by other processes executing in the system.

Why we need IPC, 

* **Information sharing:** More than one process is interested in same piece of information (ex shared file).

* **Computation speedup:** If we want some task to run faster, we can divide into subtasks and run concurrently. But its supported only in multiprocessing cores.

* **Modualarity:** Dividing systems= functions into separate processes or threads.

* **Convenience:** Single user can work on many tasks at a time. For example, editing, listening music, compiling in parallel.

IPC mechanism is provided to the coperative processes to exchange data and information between them.
There are two ways, **1. Shared memory 2. Message passing**

In shared memory, **region of memory** is shared by cooperating processes. Its difficult to implement, and 
faster method than message queues.

In Message queues, data is exchanged between the processes through messages, it is good option for smaller data, and easy to implement.
