#include "linux/init.h"
#include "linux/module.h"
#include "linux/moduleparam.h"

//Module parameters
static int value = 10;
static char* name = "Testing";

module_param(value, int, 0);
module_param(name, charp, 0);

//Initialization function
static int __init initialization_function(void)
{
    printk(KERN_ALERT "Initliazation happens here. ");
    return 0;
}

//cleanup or exit function
static void __exit cleanup_function(void)
{
    printk(KERN_ALERT "Cleanup happens here. ");
}

//calling of module init
module_init(initialization_function);

//calling of module exit
module_exit(cleanup_function);

//author
MODULE_AUTHOR("Krishna");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Experimenting basic kernel module init and exit");
MODULE_VERSION("1.0");
