#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/spinlock.h>

#define MAX_NODES 2
#define BUFFER_SIZE 100
DEFINE_MUTEX(driver_mutex_lock);
DEFINE_SPINLOCK(driver_spin_lock);

struct device_data{    
    struct cdev dev;
    u8 *read_buffer;
    u8 *write_buffer;    
    unsigned int permission;
    struct mutex read_lock;
    struct mutex write_lock;
     struct mutex mem_lock;
    bool sync_flag;
    wait_queue_head_t wq;
    u32 device_number;
    u32 interrupt_number;
};

struct driver_data
{
    struct class *driver_class;
    char *driver_name;   
    dev_t major_number;
};

struct driver_data driver_priv_data ={
    .driver_class = NULL,
    .driver_name = "samplechardriver",
    .major_number = 0,
};

//Read operation
ssize_t chdriver_read(struct file *filp, char __user *buffer, size_t count, loff_t *f_pos)
{
    int retval;
    struct device_data *dev_data = filp->private_data;
    
    pr_info("Sample_chdriver_read called \n");
    /* Dummy concept: Assume that interrupt will be triggered to inform device is available to communicate*/    
    //check for the sync flag. 
    //If not available, wait for it.
    if(!dev_data->sync_flag)
    {
        pr_err("chdriver_read: Sync bit is not set. wait for it \n");
    }
    while(!dev_data->sync_flag)
    {
        if(wait_event_interruptible(dev_data->wq, (dev_data->sync_flag == true)))
        {
            //-ERESTARTSYS
            pr_warn("chdriver_read: wait_event_interruptible interrupted \n");
        }
    }
    if(dev_data->sync_flag)
    {
        //get data
        mutex_lock(&dev_data->read_lock);
        //Dummy: assumption send request to read data from chip
        count = (count > BUFFER_SIZE) ? BUFFER_SIZE:count;
        retval = copy_to_user(buffer, dev_data->read_buffer, count);
        if(retval)
        goto copy_err;
        mutex_unlock(&dev_data->read_lock);
    }

    return count;

    copy_err:    
     mutex_unlock(&dev_data->read_lock);
    pr_err("chdriver_read: read copy failed \n");
    return retval;
}

//write function
ssize_t chdriver_write(struct file *filp, const char *buffer, size_t count, loff_t *f_pos)
{
    int retval;
    struct device_data *dev_data = filp->private_data;
    
    /* Dummy concept: Assume that interrupt will be triggered to inform device is available to communicate*/    
    //check for the sync flag. 
    //If not available, wait for it.
    if(!dev_data->sync_flag)
    {
        pr_err("chdriver_write: Sync bit is not set. wait for it \n");
    }
    while(!dev_data->sync_flag)
    {
        if(wait_event_interruptible(dev_data->wq, (dev_data->sync_flag == true)))
        {
            //-ERESTARTSYS
            pr_warn("chdriver_write: wait_event_interruptible interrupted \n");
        }
    }
    if(dev_data->sync_flag)
    {
        //get data
        mutex_lock(&dev_data->write_lock);
        count = (count > BUFFER_SIZE) ? BUFFER_SIZE:count;
        retval = copy_from_user(dev_data->write_buffer,buffer, count);
        if(retval)
        goto copy_err;
        //Dummy: assumption send request to write data to chip
        mutex_unlock(&dev_data->write_lock);
    }

    return count;

    copy_err:    
    mutex_unlock(&dev_data->write_lock);
    pr_err("chdriver_write: failed \n");
    return retval;
}

static int check_access(unsigned int available_access, unsigned int requested_access)
{
    int retval = 0;
    unsigned int access = requested_access & O_ACCMODE;

    if(((available_access == O_RDONLY) && ((access == O_WRONLY) || (access == O_RDWR))) || 
    ((available_access == O_WRONLY) && ((access == O_RDONLY) || (access == O_RDWR))))
    {
        //failed
        goto err;
    }
    else{
        pr_info("access is ok\n");
    }
    return retval;
    err:
    return -1;
}
//Open function
int chdriver_open(struct inode *iinode, struct file *filp)
{
    struct device_data* dev = container_of(iinode->i_cdev, struct device_data, dev);
    filp->private_data = dev;
    
    pr_info("Sample_chdriver_open called \n");
    //check access
    if(check_access(dev->permission,filp->f_flags))
    {       
        pr_info("Sample_chdriver_open:: failed no access. Dev permission %d, requested permission = %d \n", dev->permission, filp->f_flags & O_ACCMODE);
    }

    //allocate buffers
    mutex_lock(&dev->mem_lock);
    dev->read_buffer = kcalloc(BUFFER_SIZE, sizeof(u8), GFP_KERNEL);
    dev->write_buffer = kcalloc(BUFFER_SIZE, sizeof(u8), GFP_KERNEL);
    if((dev->read_buffer == NULL) || (dev->write_buffer == NULL))
    {
        pr_err("Sample_chdriver_open: failed to allocate memory %d\n");
        return -ENOMEM;
    }
    mutex_unlock(&dev->mem_lock);

    return 0;

}
//release function
int chdriver_release(struct inode *iinode, struct file *filp)
{
    struct device_data *dev_data = filp->private_data;    
    pr_info("Sample_chdriver_release called \n");

    mutex_lock(&dev_data->mem_lock);
    kfree(dev_data->read_buffer);
    kfree(dev_data->write_buffer);
    mutex_unlock(&dev_data->mem_lock);
    return 0;
}


struct file_operations fops={
    //define file operations
    .read = chdriver_read,
    .write = chdriver_write,
    .release = chdriver_release,
    .open = chdriver_open
};

static const struct of_device_id sample_chdrv_dt_ids[] = {
{ .compatible = "chardriver" },
{},
};

irqreturn_t interrupt_handler(int int_no, void *dev)
{
    irqreturn_t ret;
    // struct device_private_data *dev_data = (struct device_private_data *)dev;
    pr_info("sample_chardev: interrupt_handler called for int no %d\n", int_no);
    struct device_data* dev_data = (struct device_data*)dev;

    //set the sync flag
    spin_lock(&driver_spin_lock);
    dev_data->sync_flag = true;
    spin_unlock(&driver_spin_lock);
    wake_up_interruptible(&dev_data->wq);

    ret = IRQ_HANDLED;
    return ret;
}

static int sample_chardev_probe(struct platform_device *dev)
{
	int retval = 0;
    struct device *cdevice = NULL;
    struct device_data *dev_data = kmalloc(sizeof(struct device_data), GFP_KERNEL);   
    u32 prop_dev_num;
        
    dev_info(&dev->dev,"sample_chardev_probe called \n");

    if(of_property_read_u32(dev->dev.of_node,"device_number", &prop_dev_num))
    {
        dev_info(&dev->dev,"sample_chardev_probe prop_dev_num error\n ");
        goto err;
    }
    dev_data->device_number = prop_dev_num;
    dev_info(&dev->dev,"sample_chardev_probe prop_dev_num  = %d\n ", prop_dev_num);

    dev_data->interrupt_number = platform_get_irq(dev, 0);

	if(dev_data->interrupt_number < 0)
    {
        dev_info(&dev->dev, "sample_chardev_probe:: error while getting iRQ number. ret=%d \n", dev_data->interrupt_number);
        goto err;
    }
    dev_info(&dev->dev, "sample_chardev_probe:: IRQ number = %d \n", dev_data->interrupt_number);

    //request_interrupt
    retval = request_irq(dev_data->interrupt_number, interrupt_handler, 0, dev_name(&dev->dev), dev_data);
    if(retval)
    {
        dev_info(&dev->dev,"interrupt request failed. retval=%d\n ", retval);
        goto err;
    }
    else
    {
        dev_info(&dev->dev,"interrupt request succeeded \n ");
    }

    //initialization of cdev structure
    cdev_init(&dev_data->dev, &fops);  
    dev_data->dev.owner = THIS_MODULE;

    // registration with vfs
    retval = cdev_add(&dev_data->dev, MKDEV(MAJOR(driver_priv_data.major_number), dev_data->device_number), 1);
    if(retval)
    {
        dev_info(&dev->dev, "Failed at cdev_add. retvalue=%d \n", retval);
        goto err;
    }
    pr_info("Device registered with vfs \n");

    //device create
    cdevice = device_create(driver_priv_data.driver_class, NULL, MKDEV(MAJOR(driver_priv_data.major_number), dev_data->device_number) , NULL, "samchdriver-%d", prop_dev_num);
    if(IS_ERR(cdevice))
    {
        retval = PTR_ERR(cdevice);
        dev_info(&dev->dev, "Module_init: Device creation failed %d \n", retval);
        goto dev_create_failed;
    }
    dev_info(&dev->dev, "Module_init: Device created with vfs samchdriver-%d \n", dev_data->device_number);
    dev->dev.platform_data = (void*) &dev_data;
    
    //init value
    dev_data->sync_flag = false;
    dev_data->permission = O_RDWR;

    //create wait queue
    init_waitqueue_head (&dev_data->wq);    
    mutex_init(&dev_data->mem_lock);
    mutex_init(&dev_data->read_lock);
    mutex_init(&dev_data->write_lock);              
    return retval;

    dev_create_failed:    
    cdev_del(&dev_data->dev);
    err:
    return retval;

}

static int sample_chardev_remove(struct platform_device *dev)
{
    int retval = 0;
    struct device_data *dev_data = ( struct device_data *)dev->dev.platform_data;

    dev_info(&dev->dev,"sample_chardev_remove called \n");
    //destroy device
    device_destroy(driver_priv_data.driver_class,  MKDEV(MAJOR(driver_priv_data.major_number), dev_data->device_number));
    //delete device
    cdev_del(&dev_data->dev);

    //free interrupt        
    free_irq(dev_data->interrupt_number, dev_data);
    return retval;
}

static struct platform_driver sample_char_device_driver = {
	.probe		= sample_chardev_probe,
	.remove		= sample_chardev_remove,
	.driver		= {
		.name	= "SAMPLECHARDRIVER",
        .of_match_table = of_match_ptr(sample_chdrv_dt_ids),
	},
};


static int __init init_function(void)
{
    pr_info("SAMPLECHDRIVER:: INIT FUNCTION\n");
    int ret;
    ret = alloc_chrdev_region(&driver_priv_data.major_number, 0, MAX_NODES, "samplechdriver");
    if(ret < 0)
    {
        goto regerr;
    }
    pr_info("SAMPLECHDRIVER:: char device number major=%d, minor=%d\n", MAJOR(driver_priv_data.major_number), MINOR(driver_priv_data.major_number));
    driver_priv_data.driver_class = class_create(THIS_MODULE, "samplechdrv");
    if (IS_ERR(driver_priv_data.driver_class))
    {
        ret = PTR_ERR(driver_priv_data.driver_class);        
        pr_err("SAMPLECHDRIVER:: INIT FUNCTION: class creation failed ret=%d\n", ret);
        goto classerr;
    }

    
    ret = platform_driver_register(&sample_char_device_driver);
    if(ret<0)
    {
        pr_info("SAMPLECHDRIVER::platform_driver_register failed ret=%d\n", ret);
        goto err;
    }
    pr_info("SAMPLECHDRIVER:: init completed. ret=%d \n", ret);

    return ret;

    
    err:
    class_destroy(driver_priv_data.driver_class);
    classerr:
    unregister_chrdev_region(driver_priv_data.major_number, 1);
    regerr:
    return ret;
}

static void __exit exit_function(void)
{
    pr_info("SAMPLECHDRIVER:: EXIT FUNCTION\n");
    
    platform_driver_unregister(&sample_char_device_driver);  
    class_destroy(driver_priv_data.driver_class);  
    unregister_chrdev(driver_priv_data.major_number, "samplechdriver");
}

module_init(init_function);
module_exit(exit_function);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sample char driver");