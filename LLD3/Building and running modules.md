Linux module is a chunk of code that can be dynamically linked into Kernel at any point of time after system has booted. 
It can be unlinked and removed from kernel when its not needed anymore. 
Mostly linux kernel modules are devices drivers (network drivers , file systems).

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

module_init -> this function invoked when module is loaded into kernel.
Module_exit-> this function invoked when module is removed.

Printk -> similar to printf , but kernel needs its own printf function because it runs by itself, without help of c library. 
d, is linked only to the kernel, and the only functions it can call are the ones exported by the kernel; there are no libraries to link to. The printk function used in hello.c earlier, for example, is the version of printf defined within the kernel and exported to modules. It behaves similarly to the original function, with a few minor differences, the main one being lack of floating-point support. Figure 2-1 shows how function calls and function pointers are used in a module
