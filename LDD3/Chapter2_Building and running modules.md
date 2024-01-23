Linux module is a chunk of code that can be dynamically linked into Kernel at any point of time after system has booted. 
It can be unlinked and removed from kernel when its not needed anymore. 
Mostly linux kernel modules are devices drivers (network drivers , file systems).
```
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
static int hello_init(void)
{
 printk(KERN_ALERT "Hello, world\n");
 return 0;
}
static void hello_exit(void)
{
 printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(hello_init);
module_exit(hello_exit);
```
module_init -> this function invoked when module is loaded into kernel.
Module_exit-> this function invoked when module is removed.

Printk -> similar to printf , but kernel needs its own printf function because it runs by itself, without help of c library. 
d, is linked only to the kernel, and the only functions it can call are the ones exported by the kernel; there are no libraries to link to. The printk function used in hello.c earlier, for example, is the version of printf defined within the kernel and exported to modules. It behaves similarly to the original function, with a few minor differences, the main one being lack of floating-point support. Figure 2-1 shows how function calls and function pointers are used in a module.

**Concurrency in Kernel:**
-> Linux system run multiple processes, more than one of which can be trying to user your driver at the same time. Most devices are capable of interrupting the processors.
->Interrupt handler runs asynchronously and can be invoked at the same time when your deriver is trying to do somethign else.
-> Linux can run on SMP systems, your driver could be executing concurrently on more than one CPU.
-> Linux kernel code must be capable of running in different context at the same time. 
->Code must take care to access shared data in ways that prevent corruption of the data.

**Current process :**
Kernel code can refer to the current process by accessing the global item current, defined in  <asm/current.h> , which gives a pointer to struct task_struct , defined in <linux/sched.h>. The current pointer refers to the process which current executing. During the execution of the system call, the current process is the one which executed the call (open, read).

```
printk(KERN_INFO "The process is \"%s\" (pid %i)\n",
 current->comm, current->pid);
```
The command name stored in current->comm is the base name of the program file that is being executed by the current process.

* Kernel has very small stack. Don't create large automatic variables , Allocate large structures dynamically when needed.
* Function name starts with (__) generally low level component of the interface , should be used with caution.
* Kernel code can't do floating point arthmetic. (extra overhead)

**Compiling modules:**
 The file Documentation/Changes in the kernel documentation directory always lists the required tool versions.

 Fox example, helloworld program
 ```
obj-m := hello.o
```
It states that there is one module to be built from the object file hello.o. The resulting modules is named hello.ko after being built from the object file.

In case of two source files, 
```
obj-m := module.ko
module-objs := file1.o file2.o
```
Make command reuired to build your module would be 
```
make -C ~/kernel-2.6 M=`pwd` modules
```
This command change its directory to one provided with the -C option (kernel source directory). There it finds kernel top level makefile. 
M option causes that makefile to move back to your current module source directory before trying to build modules target. This target refers to the list of modules found on the obj-m variable.

sample makefile:
```
# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
 obj-m := hello.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
 KERNELDIR ?= /lib/modules/$(shell uname -r)/build
 PWD := $(shell pwd)
default:
 $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
```
**Loading and unloading modules:**
Once the kernel module is built the next step is to load the module into kernel. We can do it by using insmod. We can also use a number of command line options to assign values to parmaeters before linking it to the current kernel. This gives the user more flexibility than compile-time configuration.

In the kernel source, the sames of system calls will always start with sys_. This is useful when grepping!

Another useful utility is **modprobe**. It works like insmod but will check first to make sure that it knows all the kernel symbols being referenced. If there are any doesn't recongize, then it will look in the module search path. Insmod just fails in this case.

Modules can be removed from the kernel using **rmmod** utility. Module removal will fail when the module still in use or module removal is disallow, if its defined in the kernel configuration. There is a option to remove the module frocefully even when its busy.

**lsmod** program produces the list of modules that is currently loaded into the kernel. Some other information, such as module is making use of some other modules as well. lsmod works by reading the **/proc/modules** virtual file. Informations on currenlty loaded modules can also found in sysfs virtual filesystem /sys/module.

**Version dependency :**
Kernel interfaces often change between releases. If we are writing a module tha is intended to work with multiple versions of the kernel, we have to use macro **ifdef** to make code to build properly. 
In <linux/version.h>,

UTS_RELEASE -> defines the version of the kernel tree.

LINUX_VERSION_CODE -> Binary representation of the kernel version. For example, one byte for each part of the version number. For example, 2.6.10 is 132618 (0x02060a).

KERNEL_VERSION (major,minor,release)-> This is the macro used to build an integer version code from the individual numbers that build up a version number. For example, KERNEL_VERSION(2,6,10) expands to 132618. This macro is very useful when you need to compare the current version and a known checkpoint.

**Kernel sysbol table:**
