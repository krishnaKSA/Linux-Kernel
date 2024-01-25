#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE1_BUFFERSIZE 100
#define DEVICE2_BUFFERSIZE 100
#define MAX_BUFFER_SIZE 100
#define MAX_DEVICES 2



//devices buffer
char dev1_buffer[DEVICE1_BUFFERSIZE];
char dev2_buffer[DEVICE2_BUFFERSIZE];

//Device private data
struct device_private_data
{
    char* serial_number;
    int size;
    int permission;
    char* buffer;
    struct cdev chdriver_cdev;
};

//Driver private data
struct chdriver_private_data
{
    struct device_private_data devices[MAX_DEVICES];
    //sysfs class 
    struct class* chdriver_class;
    struct device* chdriver_device;
    dev_t device_number; //u32
    int number_of_devices;
};


struct chdriver_private_data chdriver_data;

//Read operation
ssize_t chdriver_read(struct file *filp, char __user *buffer, size_t count, loff_t *f_pos);
//write function
ssize_t chdriver_write(struct file *filp, const char *buffer, size_t count, loff_t *f_pos);
//Open function
int chdriver_open(struct inode *iinode, struct file *filp);
//release function
int chdriver_release(struct inode *iinode, struct file *filp);
//close function
int chdriver_close(struct inode *iinode, struct file *filp);
//fileoperations of the driver
struct file_operations chdriver_fops = {
    //define file operations
    .read = chdriver_read,
    .write = chdriver_write,
    .release = chdriver_release,
    .open = chdriver_open
};


static void init_all_devices(void)
{
    chdriver_data.number_of_devices = MAX_DEVICES;

    chdriver_data.devices[0].serial_number ="12";
    chdriver_data.devices[0].permission =O_RDONLY;
    chdriver_data.devices[0].buffer = dev1_buffer;
    chdriver_data.devices[0].size = DEVICE1_BUFFERSIZE;


    chdriver_data.devices[1].serial_number ="13";
    chdriver_data.devices[1].permission =O_RDWR;
    chdriver_data.devices[1].buffer = dev2_buffer;
    chdriver_data.devices[1].size = DEVICE2_BUFFERSIZE;
}

//Module init function
static int __init char_driver_init_function(void)
{    
    int i = 0;
    int ret = 0;
    
    pr_info("Module init called \n");

    //Init driver private data
    init_all_devices();

    //STEP 1:: create device number 
    ret = alloc_chrdev_region(&chdriver_data.device_number, 0, MAX_DEVICES, "chdriver"); //chdriver is the name of the device number range.
    pr_info("Module init: device number created MAJOR=%d  \n", MAJOR(chdriver_data.device_number));

    //STEP 2: create sys class structure
    chdriver_data.chdriver_class = class_create(THIS_MODULE, "chdriver");
    pr_info("Module init: sysfs class created");

    //STEP 3: Create devices
    for(i = 0; i < MAX_DEVICES; i++)
    {
        //STEP 3.1:: initialization of cdev structure
        cdev_init(&chdriver_data.devices[i].chdriver_cdev, &chdriver_fops);  
        chdriver_data.devices[i].chdriver_cdev.owner = THIS_MODULE;

        //STEP 3:2:: registration with vfs
        ret = cdev_add(&chdriver_data.devices[i].chdriver_cdev,chdriver_data.device_number+i, 1);
        if(ret < 0)
        {
            pr_err("Module_init::Failed at cdev_add \n");
        }

        pr_info("Module_init: Device registered with vfs . Device number = %d, index = %d\n", MAJOR(chdriver_data.device_number), i);

        //device create
        chdriver_data.chdriver_device = device_create(chdriver_data.chdriver_class, NULL, chdriver_data.device_number+i, NULL, "chdriver-%d", i+1);
        pr_info("Module_init: Device created with vfs . Device number = %d\n", MAJOR(chdriver_data.device_number)); 
    }

    return 0;
}

//Module exit function
static void __exit char_driver_exit_function(void)
{
    //destroy device
    int i;
    for(i=0; i<MAX_DEVICES; i++)
    {
        device_destroy(chdriver_data.chdriver_class, chdriver_data.device_number+i);
    }

    class_destroy(chdriver_data.chdriver_class);

    for(i=0; i<MAX_DEVICES; i++)
    {
        cdev_del(&chdriver_data.devices[i].chdriver_cdev);
    }

    unregister_chrdev_region(chdriver_data.device_number, MAX_DEVICES);

}

//Read operation
ssize_t chdriver_read(struct file *filp, char __user *buffer, size_t count, loff_t *f_pos)
{
    struct device_private_data* device_data = (struct device_private_data*)filp->private_data;
    pr_info("chdriver_read: read requested for %d bytes , f_pos =%d \n", count, *f_pos);
    //check the count
    if((count + *f_pos) > (device_data->size))
    {
        pr_err("chdriver_read:: count is greater than available data \n");

        count = device_data->size - *f_pos; //find the remaining data can be read
    }

    //send to user
    if(copy_to_user(buffer,&device_data->buffer[*f_pos], count))
    {
        return -EFAULT;
    }

    //adjust fpos position
    *f_pos += count;
    pr_info("chdriver_read:: read completed for bytes %zu,  fpos updated to %lld\n", count, *f_pos);
    return count;
}

//write function
ssize_t chdriver_write(struct file *filp, const char *buffer, size_t count, loff_t *f_pos)
{
    struct device_private_data* device_data = (struct device_private_data*)filp->private_data;
    pr_info("chdriver_write:: write requested for bytes %zu,  f_pos updated to %lld\n", count, *f_pos);

    if(device_data->size < (count + *f_pos))
    {
        count = device_data->size - *f_pos;
    }

    if(!count)
    {
        pr_err("Invalid count =%d \n", count);
    }

    if(copy_from_user(&device_data->buffer[*f_pos], buffer, count))
    {
        return -EFAULT;
    }


    *f_pos += count;

    pr_info("chdriver_write:: write completed for bytes %d,  fpos updated to %lld\n", count, *f_pos);
    return count;

}

//Open function
int chdriver_open(struct inode *iinode, struct file *filp)
{
    struct device_private_data* device_data;

    //Check which device file open was requested by the user
    pr_info("Device file %d:%d was opened by the user \n", MAJOR(iinode->i_rdev), MINOR(iinode->i_rdev));

    //get the device private data
    device_data = container_of(iinode->i_cdev, struct device_private_data, chdriver_cdev);

    filp->private_data = device_data;

    //check permission
    //device_data->permission filp->f_mode 
    return 0;
}

//release function
int chdriver_release(struct inode *iinode, struct file *filp)
{
    pr_info("chdriver_release: called \n");
    return 0;
}



//Module init and exit
module_init(char_driver_init_function);
module_exit(char_driver_exit_function);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Char driver implementation");