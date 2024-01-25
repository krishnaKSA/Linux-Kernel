#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>

#define TYPE 'a'
#define MAX_NR 2

//Define IOCTL commands
#define IOCTLWRITE _IOW(TYPE, 1, unsigned int* )
#define IOCTLREAD _IOR(TYPE, 2, unsigned int*)

//device value changed by ioctl operation
unsigned int budrate = 100;

//Device number
dev_t device_number;
//device structure
struct cdev ioctl_dev;
//sys class structure
struct class* ioctl_class;
//device 
struct device* ioctl_device;

ssize_t ioctl_read(struct file *filep, char __user *buffer, size_t, loff_t *f_pos); 
ssize_t ioctl_write(struct file *filep, const char __user *buffer, size_t, loff_t *f_pos);
int ioctl_open(struct inode *iinode, struct file *filep); 
int ioctl_release(struct inode *iinode, struct file *filep);
long ioctl_handler(struct file *filep, unsigned int cmd, unsigned long arg);

struct file_operations fops={
    .open = ioctl_open,
    .read = ioctl_read,
    .write = ioctl_write,
    .release = ioctl_release,
    .unlocked_ioctl = ioctl_handler
};

//init function
static int __init init_ioctl_driver(void)
{
    int ret;

    //step1: Device number creation
    ret = alloc_chrdev_region(&device_number, 0, 1, "ioctldriver");
    if(ret < 0)
    goto devnumerr;

    //Device initialization
    cdev_init(&ioctl_dev, &fops);
    ioctl_dev.owner = THIS_MODULE;

    //device registation with vfs
    ret = cdev_add(&ioctl_dev, device_number, 1);
    if(ret<0)
    goto regsfailed;    

    //class creation
    ioctl_class = class_create(THIS_MODULE, "ioctldriver");

    //device create
    ioctl_device = device_create(ioctl_class, NULL, device_number, NULL, "ioctldriver-%d", 0);

    pr_info("IOCTL: init_ioctl_driver: completed \n");    
    return 0;

    regsfailed:
    unregister_chrdev_region(device_number, 1);
    devnumerr:
    return -1;
}

//exit function
static void __exit exit_ioctl_driver(void)
{
    //destory device
    device_destroy(ioctl_class,device_number);
    class_destroy(ioctl_class);
    cdev_del(&ioctl_dev);
    unregister_chrdev_region(device_number, 1);
    pr_info("IOCTL: exit_ioctl_driver completed\n");
}

ssize_t ioctl_read(struct file *filep, char __user *buffer, size_t size, loff_t *f_pos)
{
    pr_info("IOCTL: ioctl_read  called \n");
    return size;
}

ssize_t ioctl_write(struct file *filep, const char __user *buffer, size_t size, loff_t *f_pos)
{
    pr_info("IOCTL: ioctl_write  called \n");
    return size;
}

int ioctl_open(struct inode *iinode, struct file *filep)
{
    pr_info("IOCTL: ioctl_open  called \n");
    return 0;
}

int ioctl_release(struct inode *iinode, struct file *filep)
{
    pr_info("IOCTL: ioctl_release  called \n");
    return 0;
}

long ioctl_handler(struct file *filep, unsigned int cmd , unsigned long arg)
{
    if(_IOC_TYPE(cmd) != TYPE)
    {
        pr_err("IOCTL: ioctl_handler: invalid type received \n");
        return -ENOTTY;
    }
    if(_IOC_NR(cmd) > MAX_NR)
    {
        pr_err("IOCTL: ioctl_handler: invalid NR received \n");
        return -ENOTTY;
    }

    switch(cmd)
    {
        case IOCTLWRITE:
        {
            pr_info("IOCTL: ioctl_handler IOCTLWRITE called \n");
            if(copy_from_user(&budrate, (unsigned int*)arg, sizeof(budrate)))
            {
                pr_err("IOCTl: error occurred in copy_from_user\n");
            }
            else
            {
                pr_info("iOCTL:ioctl_handler: write data sucessfully. budrate =%d\n", budrate );
            }
        }
        break;
        case IOCTLREAD:
        {
             pr_info("IOCTL: ioctl_handler IOCTLREAD called \n");
            if(copy_to_user((unsigned int*)arg, &budrate, sizeof(budrate)))
            {
                pr_err("IOCTl: error occurred in copy_to_user\n");
            }
            else{
                pr_info("iOCTL:ioctl_handler: read data sucessfully. budrate =%d\n", budrate );
            }
        }
        break;
        default:
        break;
    }
    return 0;
}


module_init(init_ioctl_driver);
module_exit(exit_ioctl_driver);

MODULE_DESCRIPTION("IOCTL DRIVER");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ioctl driver");

