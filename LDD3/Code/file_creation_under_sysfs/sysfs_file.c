#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

#define BUFFER_SIZE 10
char *data_buffer;
DEFINE_MUTEX(buf_lock);
int val=0;

static int   __init init_function(void);
static void  __exit exit_function(void);

static ssize_t read_info(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    mutex_lock(&buf_lock);
    strncpy(buf, data_buffer, BUFFER_SIZE);
    mutex_unlock(&buf_lock);

    return BUFFER_SIZE;
}

static ssize_t write_info(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    mutex_lock(&buf_lock);
    memset(data_buffer, 0, BUFFER_SIZE);
    count = (count > BUFFER_SIZE)? BUFFER_SIZE: count;
	strncpy(data_buffer, buf, count);
    mutex_unlock(&buf_lock);

    return count;
}

static struct kobj_attribute attribute = __ATTR(data_buffer, 0660, read_info, write_info);

struct kobject *kobj=NULL;
static int __init init_function()
{

    int retval;

    //step1:: 
    kobj = kobject_create_and_add("sysfs_buf", kernel_kobj);
    if(kobj == NULL)
    goto objcreate_failed;

    data_buffer = kcalloc(BUFFER_SIZE, sizeof(char), GFP_KERNEL);
    if(data_buffer==NULL)
    goto memalloc_failed;

    retval = sysfs_create_file(kobj, &attribute.attr);
	if (retval) {
		pr_err("init_funtion: sysfs create failed\n");
		goto sysfs_failed;
	}
    
    return 0;

    sysfs_failed:
    kfree(data_buffer);
    memalloc_failed:
    kobject_put(kobj);
    objcreate_failed:
    pr_err("init_function: memory allocation failed \n");
    return -ENOMEM;
}


static void __exit exit_function()
{
    kfree(data_buffer);
    kobject_put(kobj);
}




module_init(init_function);
module_exit(exit_function);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sysfs file creation");
