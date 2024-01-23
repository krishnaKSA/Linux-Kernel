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

**Process state:**
As a process executes, it changes state. The state of a process is defined in part by the current activity of that process. 
A process may be in one of the following states:
• New. The process is being created.
• Running. Instructions are being executed.
• Waiting. The process is waiting for some event to occur (such as an I/O
completion or reception of a signal).
• Ready. The process is waiting to be assigned to a processor.
• Terminated. The process has finished execution

**Process Control Block**
Each process is represented in the operating system by a process control block (PCB)—also called a task control block.

![Process-state-transition-diagram](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/75e44a17-5816-4e4b-83de-16964a2f346a)

• **Process state.** The state may be new, ready, running, waiting, halted, and so on.

• **Program counter.** The counter indicates the address of the next instruction to be executed for this process.

• **CPU registers.** The registers vary in number and type, depending on the computer architecture. They include accumulators, index registers, stack pointers, and general-purpose registers, plus any condition-code  information. Along with the program counter, this state information must be saved when an interrupt occurs, to allow the process to be continued
correctly afterward.

• **CPU-scheduling information.** This information includes a process priority, pointers to scheduling queues, and any other scheduling parameters.

• **Memory-management information.** This information may include such items as the value of the base and limit registers and the page tables, or the segment tables, depending on the memory system used by the operating system.

![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/0d1359b7-8fee-4e8a-9427-1cac0cec4cc1)


![R](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/b98a982a-d4e6-425e-9afa-01be5af8f042)




