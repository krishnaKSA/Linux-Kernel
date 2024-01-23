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
After insmod has loaded the program, the module can access the kernel's public symbols. KERN_ALERT is used to make the message show up with high priority (otherwise it might not show up anywhere).

**Difference Between a Kernel Module and an Application**

-> While not all applications are event-driven, every kernal module is
-> An application can be lazy in how it is destroyed with releasing resources, while the exit of a module must carefully undo everything from the init() function or the pieces will remain until the system is rebooted
-> Faults are handled safely in applications, but they can easily cause a whole system crash in a kernel module
-> A module runs in kernel space, where an application runs in user space
Unloading is nice - it allows you to develop without rebooting the system every time you make changes to it. The only functions a kernel module can use are the ones exported by the kernel. There are no libraries to link to.

Due to the lack of libraries, the only includes should come from the kernel source tree, and usual c header files should never be used.

**Kernal Space versus User Space
Differences:**

Privilege Level - User space is low, while kernal space has high permission level
Each mode has its own memory mapping and address space
User space will go into kernel space for system calls and interrupts

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

**Kernel symbol table:**
Insmod resoles undefined symbols against the table of public kernel symbols. Table contains the address of glocal kernel items - functions and variables that are needed to implement modularized drivers.
When the module i sloaded, any symbol exported by the module becomes part of kernel symbol table.In the usual case, module implement its own functionality without need to export the symbol at all. we need to export symbols, however whenever other modules may benifit from using them.

New modules can use symbols exported by your module, and you can stack new modules on top of other modules. 
msdos filesystem reiles on symbols exported by the fat module, and each input usb device module stacks on b the usbcore and input modules.

This concept of stacking is useful in complex projects. It is implemented in the mainstream kernel. This is 
where moprpbes comes in handy -it can automatically load insmod for any dependent modules we need to run to complet the stack. 

The following macros can be used to export symbols:
```
EXPORT_SYMBOL(name);
EXPORT_SYMBOL_GPL(name);
```
The GPL version makes the symbol available to GPL-licensed modules only. This variable is stored in a special part of the module executable (an “ELF section”)that is used by the kernel at load time to find the variables exported by the module.

**Preliminaries:**
Most kernel includes large number of header files.Just above every module has the following heades:

```
#include <linux/module.h>
#include <linux/init.h>
```

module.h -> contains many definitions of symbols and functions needed by loadable modules.
init.h -> specify our initialization and clean up functions. 
Most modules includes moduleparm.h to enable passing of parameters to the module at the load time.

For good practice, we should also include the liscense of our module. 
Example:

MODULE_LICENSE("GPL");
Different Licenses Recognized:

GPL - for any version of the GNU General Public License
GPL v2 - for GPL version two only
GPL and additional rights
Dual BSD/GPL
Dual MPL/GPL
Proprietary
Unless our module is specifically marked as under a free license, it is assumed proprietary and the kernel is then "tainted." Don't do this. Nobody likes a tainted kernel.

There is a newer license that I will be using: the Dual MIT/GPL license. I don't want to certify any software I write, but I want everyone else to be able to use it for free. It also needs the GPL license because it is derivative work of the kernel, which is under the GPLv2 license I believe.

Other descriptive definitions:

MODULE_AUTHOR - stating who wrote the module
MODULE_DESCRIPTION - a human-readable statement of what the module does
MODULE_VERSION - for a code revision number; see the comments in <linux/module.h> for the conventions to use in creating version strings
MODULE_ALIAS - another name by which this module can be known
MODULE_DEVICE_TABLE - to tell user space about which devices the module supports
Module declarations can appear anywhere outside of a function, but recently are being put at the end of the file.


**Initialization and shutdown:**

The module initialization functions registers any facility(new functionality (whole driver or software abstarction)) that can used by the application. 

Definition of initialization function always looks like this:
```
static int __init initialization_function(void)
{
 /*initialization code here */
}
module_init(initialization_function);
```
This function should be static, so that its not visible outside of a specific file. 

This __init token in the definition is the hint to kernel that this function only has to bed used at the time of initialization. The memoory used for initialization function is dropped after the module is loaded to free up memory for other uses.
__initdata token used for data only used during initialization.

Module_init is mandatory. This macro adds a spection section in the module object code station where module initialization function is to be found. Without this definition module initialization function never called. 

Some facilities the modules can register inludes device, filesystems, crypto transforms, and more. For each of these facilites , there is a kernel function that accomplishes the registration. The argument passed to the kernel registration functions are usually pointers to data structures describing new facility and the name of the facility being registered. This data structure typically contains the pointers to module funtions.

**The Cleanup function:**
THe cleanup function unregisters interfaces and returns all the resources to the system beofre the module is removed.
The definition of the exit function is, 

```
static void __exit cleanup_function(void)
{
 /* cleanup code here */
}
module_exit(cleanup_function);
```

The cleanup value returns void. The __exit modifier marks the code as existing for module unload only. This causes the compiler to put it in a special ELF section. A kernel can be configured to disallow the unloading of modules, in which case functions marked with __exit are discarded. module_exit declaration is necessary to let the kernel find your cleanup function. If there is no cleanup function, the kernel will not allow your module to be unloaded.


**Error handling during initialization:**

Module code must check the return values to ensure requested operations has successfully completed.
If an error occurs, 
1. Check if module can continue initialize anyway.
2. If it can continue, it might still able to opeate with degraded functionality if necessary.Always try to continue operation with what is working when error happens.
3. If it can't continue, then we have to undo the registation activities before the failures.  If unregistration fails, the kernel is left in an unstable state because it contains pointers that no longer exist.

Error recovery can be handled in goto statement.

Example code for error handling:
```
int __init my_init_function(void)
{
 int err;
 err = registerone(ptr1, "one");
 if(err)
 goto failone;
 err = registertwo(ptr2, "two");
 if(err)
 goto failtwo;
 err = registerthree(ptr3, "three");
 if(err)
 goto failthree;
 return 0;
 failthree: unregister_two(ptr2,"two");
 failtwo:unregister_one(ptr1, "one");
 failone: return err;
}
```
**Error Codes:**
Returning err is used in the previous example. Error codes in the kernel are negative numbers belonging to a set defined in <linux/errno.h>. To make your own error codes, include this header file to use symbolic values like -ENODEV, -ENOMEM, and so on.

If the initialization, amnd cleanup are more complicated than few items, then goto approach is difficult to manage. In this case, call cleanup function from within the initialization fucntion wherever error occurs.
The cleanup function must check the status of each item before undoing its registration.

```
struct something *item1;
struct somethingelse *item2;
int stuff_ok;

void my_cleanup(void)
{
 if (item1)
 release_thing(item1);
 if (item2)
 release_thing2(item2);
 if (stuff_ok)
 unregister_stuff( );
 return;
 }
 
int __init my_init(void)
{
 int err = -ENOMEM;
 item1 = allocate_thing(arguments);
 item2 = allocate_thing2(arguments2);
 if (!item2 || !item2)
 goto fail;
 err = register_stuff(item1, item2);
 if (!err)
  stuff_ok = 1;
 else
 goto fail;
 return 0; /* success */
 fail:
 my_cleanup( );
 return err;
}
```
**Module-Loading Races:**

-> another part of the kernel make use of the facility immediately after the registration has completed. Ensure all the internel registration is complete, before you register any facility of your code. 
-> if the initialization function fails, consider what happens if some part of the kernel was already suing some facility of your module has registered already.  If this happens, consider not failing your module if something useful has already come out of it. If it must fail, keep this in consideration for how to handle that failure.

**Module Parameters:**

Parameters can be assigned during the load time by insmod or modprobe, later it can be read from configuration file <u>(/etc/modprobe.conf)</u>.
command line ```insmod hellop howmany=10 whom="Mom" ```

insmod can change module parameters, the module must make them available.Parameters are declared with the module_param macro which is defined moduleparam.h.

module_param takes three parameters: name of the variable, its type, permission mask to appcomplish sysfs entry.
```
static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);
```
```
module_param_array(name,type,num,perm);
```
The final module_param field is a permission value; you should use the definitions
found in <linux/stat.h>. This value controls who can access the representation of the
module parameter in sysfs.If perm is set to 0, there is no sysfs entry at all; otherwise,
it appears under /sys/module* with the given set of permissions. Use S_IRUGO for a parameter that can be read by the world but cannot be changed; S_IRUGO|S_IWUSR allows root to change the parameter. Note that if a parameter is changed by sysfs, the value of that parameter as seen by your module changes, but your module is not notified in any other way. You should probably not make module parameters writable, unless you are prepared to detect the change and react accordingly.

**Quick reference:**
```
insmod
modprobe
rmmod
      User-space utilities that load modules into the running kernels and remove them.

#include <linux/init.h>
module_init(init_function);
module_exit(cleanup_function);
      Macros that designate a module’s initialization and cleanup functions.

__init
__initdata
__exit
__exitdata
      Markers for functions (__init and __exit)and data (__initdata and __exitdata) that are only used at module initialization or cleanup time. Items marked for initialization may be discarded once initialization completes; the exit items may be discarded if module unloading has not been configured into the kernel. These markers work by causing the relevant objects to be placed in a special ELF section in the executable file.

#include <linux/sched.h>
       One of the most important header files. This file contains definitions of much of
the kernel API used by the driver, including functions for sleeping and numerous variable declarations.

struct task_struct *current;
The current process.
current->pid
current->comm
     The process ID and command name for the current process.

obj-m
A makefile symbol used by the kernel build system to determine which modules
should be built in the current directory.

/sys/module
/proc/modules
      /sys/module is a sysfs directory hierarchy containing information on currentlyloaded modules. /proc/modules is the older, single-file version of that information. Entries contain the module name, the amount of memory each module occupies, and the usage count. Extra strings are appended to each line to specify flags that are currently active for the module.

vermagic.o
      An object file from the kernel source directory that describes the environment a module was built for.

#include <linux/module.h>
      Required header. It must be included by a module source.

#include <linux/version.h>
     A header file containing information on the version of the kernel being built.

LINUX_VERSION_CODE
     Integer macro, useful to #ifdef version dependencies.

EXPORT_SYMBOL (symbol);
EXPORT_SYMBOL_GPL (symbol);
      Macro used to export a symbol to the kernel. The second form exports without using versioning information, and the third limits the export to GPL-licensed modules.

MODULE_AUTHOR(author);
MODULE_DESCRIPTION(description);
MODULE_VERSION(version_string);
MODULE_DEVICE_TABLE(table_info);
MODULE_ALIAS(alternate_name);
     Place documentation on the module in the object file.

module_init(init_function);
module_exit(exit_function);
      Macros that declare a module’s initialization and cleanup functions.

#include <linux/moduleparam.h>
     module_param(variable, type, perm);
     Macro that creates a module parameter that can be adjusted by the user when the module is loaded (or at boot time for built-in code). The type can be one of bool, charp, int, invbool, long, short, ushort, uint, ulong, or intarray.

#include <linux/kernel.h>
      int printk(const char * fmt, ...);
      The analogue of printf for kernel code.
```










