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

In fork(), child process ****


