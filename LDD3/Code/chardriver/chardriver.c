#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define MAX_BUFFER_SIZE 100
#define MAX_DEVICES = 4

//Device number
dev_t device_number; //uint32t number


//Device buffer
char device_buffer[MAX_BUFFER_SIZE];

//Cdev structure
struct cdev chdriver_cdev[MAX_DEVICES];

//fileoperations of the driver
struct file_operations chdriver_fops;

//sysfs class 
struct class* chdriver_class = NULL;

struct device* chdriver_device = NULL;


//Module init function
static int __init char_driver_init_function(void)
{
    pr_info("Module init called \n");
    //STEP 1:: create device number (4 instances are created)
    int err = alloc_chrdev_region(&device_number, 0, MAX_DEVICES, "chdriver"); //chdriver is the name of the device number range.

    pr_info("Module init: device number created MAJOR=%d  \n", MAJOR(device_number));
    //define file operations
    chdriver_fops.read = chdriver_read;
    chdriver_fops.write = chdriver_write;
    chdriver_fops.release = chdriver_release;
    chdriver_fops.open = chdriver_open;
    chdriver_fops.close = chdriver_close;

    //STEP 2: create sysfs class structure
    chdriver_class = create_class(THIS_MODULE, "chdriver");
    pr_info("Module init: sysfs class created");

    //STEP 3: Create devices
    for(int i = 0; i < MAX_DEVICES; i++)
    {
        //STEP 2:: device driver registration with VFS
        cdev_init(&chdriver_cdev[i], &chdriver_fops); //initialization    
        chdriver_cdev[i].owner = THIS_MODULE;

        //registration with vfs
        err = cdev_add(&chdriver_cdev[i], MKDEV(MAJOR(device_number), i), 1);
        pr_info("Module_init: Device registered with vfs . Device number = %d\n", MAJOR(device_number));

        //device create
        chdriver_device = device_create(chdriver_class, NULL, device_number, NULL, "chdriver%d", i);
        pr_info("Module_init: Device registered with vfs . Device number = %d\n", MAJOR(device_number)); 
    }

    return 0;
}

//Module exit function
static void __exit char_driver_exit_function(void)
{
    //destroy device
    for(int i=0; i<MAX_DEVICES; i++)
    {
        device_destory(chdriver_class, MKDEV(MAJOR(device_number), i));
    }

    class_unregister(chdriver_class);
    class_destory(chdriver_class);

    cdev_delete(chdriver_cdev);

    unregister_chrdev_region(device_number, MIONR_MASK);

}

//Read operation
ssize_t chdriver_read(struct file *filp, char __user *buffer, size_t count, loff_t *f_pos)
{
    pr_info("chdriver_read: read requested for %d bytes , f_pos =%d \n", count, *f_pos);
    //check the count
    if((count + *f_pos) > (MAX_BUFFER_SIZE))
    {
        pr_error("chdriver_read:: count is greater than available data \n");

        count = MAX_BUFFER_SIZE - *f_pos; //find the remaining data can be read
    }

    //send to user
    if(copy_to_user(buffer, device_buffer[*f_pos], count))
    {
        return EFAULT;
    }

    //adjust fpos position
    *f_pos += count;
    pr_info("chdriver_read:: read completed for bytes %d,  fpos updated to %d\n", count, *f_pos);
    return count;
}

//write function
ssize_t chdriver_write(struct file *filp, const char *buffer, size_t count, loff_t *f_pos)
{
    pr_info("chdriver_write:: write requested for bytes %d,  f_pos updated to %d\n", count, *f_pos);

    if(MAX_BUFFER_SIZE < (count + *f_pos))
    {
        count = MAX_BUFFER_SIZE - *f_pos;
    }

    if(!count)
    {
        pr_error("Invalid count =%d \n", count);
    }

    if(copy_from_user(device_buffer[*f_pos], buffer, count))
    {
        return EFAULT;
    }

    *f_pos += count;

    pr_info("chdriver_write:: write completed for bytes %d,  fpos updated to %d\n", count, *f_pos);
    return count;

}

//Open function
int chdriver_open(struct inode *iinode, struct file *filp)
{
    //TODO - Check for multiple user access
    return 0;
}

//release function
int chdriver_release(struct inode *iinode, struct file *filp)
{
    pr_info("chdriver_release: called \n");
    return 0;
}

//close function
int chdriver_close(struct inode *iinode, struct file *filp)
{
    pr_info("chdriver_close: called \n");
    return 0;
}


//Module init and exit
module_init(char_driver_init_function);
module_exit(char_driver_exit_function);

MODULE_DESCRIPTION("Char driver implementation");
