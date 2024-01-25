https://linux-kernel-labs.github.io/refs/heads/master/lectures/intro.html

**Kernel responsibilities:**

1. Process management:
	->In charge of creating and destroying process.
	-> communication between input and output world
	-> communication between different processes like pipe, ipc, signals
	-> in addition, scheduler, which controls how processes share CPU	
2. Memory management:
    -> Creates virtual address space on top of limited resources (malloc, free)
3.File system:
	-> FAT, ext3
4. Device control 
	-> device control operations are performed by device code that is specific to the device being addressed. This code called device driver.		

User space -application runs in user space

Kernel - has full control of CPU

local interrupts of cpu can be enabled or disabled in kernel mode, if this operation executes in user mode, kernel will handle it.

The kernel space is the memory area that is reserved to the kernel while user space is the memory area reserved to a particular user process.
The kernel space is accessed protected so that user applications cannot access it directly, while user space can be directly accessed from code running in kernel mode.
System kernel is responsible for access and share the Hardware in a secure way and fine way with multiple application. 
Kernel code -> Core kernel -> common one. It has subsystems like process management, memory management, networking, file system.

![1](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/409fe845-869b-4b98-a4d3-f8873c4a138c)

Device drivers code -> specific to device.
Kernel API -> system call interfaces

**Monolithic kernel:**

Monolithic kernel has no access protection between various subsystems. Public functions can be called between subsystems.
 ![ditaa-3dc899167df5e16a230c434cf5d6964cb5868482](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/cf65a40c-6e6c-4ff5-89fa-8f237741c54d)

**MicorKernel:**
Large part of the kernel protected from each other. Most of the kernel subsystem runs in user space, 
In microkernel, IPC, Scheduler, memory management will run from kernel space. Basic memory management system to protect between different services and applications.
One of the advantages is, if any service is crashed no need to start all the server, only restart the service which is crashed.
Offers memory protection between different services. Now simple call between two services in monolithic, goes through IPC, scheduler which increases the cost. 
![ditaa-c8a3d93d0109b7be6f608871d16adff4aaa933da](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/bbb40119-eaf2-4f7b-b145-d4da34e7c19f)

**Address Space:**
The physical address space is referred to the way how RAM and other device memory visible on the memory bus. In intel 32-bit architecture, RAM is mapped to lower part of the physical address space, and graphics is mapped to higher part of the physical address space.
Virtual address space is referred to the way how CPU sees the memory is the virtual memory module is activated or paging enabled or protected mode enabled.
Kernel is responsible to setup the mapping between virtual address space and physical address space.
Virtual address space


**Kernel address space:**

Process(user) address space (starts from 0 and continuous memory)
Typical implementation of user and kernel space is one where virtual address is shared between user process and kernel.
 
Interrupt context runs in kernel mode as a result of interrupt trigger, this code shouldn’t have any blocking call or calls to user space. 
Process context runs in process code (user space) or in kernel by executing system call.
![ditaa-a5f93e0d17ccdc2ba24828b620d7227f7fc75e33](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/ef66c9a5-1f39-4481-8ba5-488783f2d390)


**Multitasking:**

Multitasking is the ability of operating system to simultaneously executes multiple programs. It does by switching between running processes.
- Cooperative multitasking -> program will run, and voluntarily it gives up the CPU to OS, which will then schedule another program. 
+ Preemptive Multitasking -> Kernel will enforce strict timings limits for each process. So that all the process gets fair chance to run. 
- Preemptive kernel -> If a process can be preempted while running in a kernel mode. But if the kernel is non preemptive, even then it can support preemptive multitasking.

**Asymmetric Multiprocessing (ASMP)**
- It’s a way of supporting multiple processors (cores) by a kernel where processor is dedicated to the kernel, and other processors runs user space programs.
+ The disadvantage of this approach is that kernel throughput (system calls, interrupt handling) doesn’t scale with number of processors. Hence typical processes frequently use system calls.

 ![ditaa-cb16db58a2489307b74d4f70256a48c81c65f6c6](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/c82c989a-7798-4ab0-b2a6-285e0b78db2e)

**Symmetric Multiprocessing:**
- Kernel can run on any of the existing processes as a user process. It creates a race condition in the kernel, if two processes run kernel function that access same memory location.
+ In order to support SMP the kernel must implement synchronization primitives (spin lock) to guarantee that only one processor is executing a critical section.
![ditaa-08aff771b3ff7a5525df7b0c090e28c836502788](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/6876a152-a3ab-4d98-a9a0-9e903e4bb82b)

 
**CPU scalability:**

It refers how well performance scales with the number of cores. 
- Use of lock free algorithms when possible.
+ Use fine grained locking for high contention areas.
- Attention to algorithm complexity.

**Linux Kernel Architecture:**


![ditaa-b9ffae65be16d30be11b5eca188a7a143b1b8227](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/eb9114c4-2cb5-40e6-bbdd-4f93e105ab4e)
 



