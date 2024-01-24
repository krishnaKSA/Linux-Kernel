
Step Module_init: connection establish between device file access and driver.
        * Create device number.

               Device number generated is dynamically. 
               alloc_chrdev_region();

![Figure-1-17](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/4b08771a-7e18-4fe1-83c5-cea037b981e7)
        
        * Create device files

               class_create();
               device_create();
        
        * Make a char device registation with the VFS(CDEV_ADD)

               cdev_init();
               cdev_add();
               
![download](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/e65bff30-0399-4fe4-90db-4f1c75d78d55)

cdev_init takes two arguments. one is cdev structure, another one file_operations struct.

cdev structure has pointer *ops which points to the file operation structure of the driver.

Owner -> pointer to the module that owns the structure, this should be initialized to THIS_MODULE.
               

![download](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/9301ea65-6c5f-49d7-a5f3-d1920198313d)

Fileopeartions struct has function pointers which has to be mapped to the drivers functions.

![Figure-2-26](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/9c7f7c71-a0f2-4dbe-89da-12d4d3937cd3)

        
* Implemenet the drivers file operation methods for open, read, write, llseek, etc.,  

![Figure-3-23](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/8647a552-f4d8-4353-a4f6-5db6f79f48a0)

In this case, when open system call invoked by user, vfs opens a file by creating a new file object and link it to corresponding inode object.

![Figure-4-19](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/fc984bc0-0387-4e00-8195-0e3dba9b8fd0)

struct inode -> holds general information about the file.

struct file-> tracks the interaction on an opened file by the user process.

Irrespective of the either device file or regular file, innode object is created and store in memory when new file get created. VFS uses innode number to indentify the file not by filename. This file object store the information about the interaction between the user process and file.

**FILE OBJECT** : When ever file is opened, file object is created in the kernel sapce. This will be in kernel space, and its deleted by vfs when file is closed.

**Device File**: When device file is created, VFS invokes **init_special_inode** function.Inode object is created. i_rdev is initialized(device number). That means, device number of the inode object is isnitialized with the device number of the newly created device file.

![Figure-5-18](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/5001b98d-c7b7-44be-b8d5-9ff0ef50a8ff)

![Figure-6-14](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/c575dc09-cc00-4955-bfde-b64845e529ad)

![Figure-7-11](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/42697ce8-d4d4-495f-89b3-17d626fe0db0)

![Figure-8-8](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/688b40bd-66fc-4fcb-9588-7f3f9f020deb)

![Figure-9-7](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/c5035a8f-f9a8-4215-b2c4-4f540d3fe05e)

 **(kobj = kobj_lookup(cdev_map, inode->i_rdev, &idx). ** It a look up for cdev which was added using cdev_add() with VFS. Its works by device number.

 container_of macro gives pointer to the parent object. cdev is the parent of kobject.
```
new = container_of(kobj, struct cdev, kobj);
p = inode->i_cdev;
if (!p)
{
 inode->i_cdev = p = new;
}
 fops = fops_get(p->ops);
 replace_fops(filp, fops);
 if (filp->f_op->open)
  {	ret = filp->f_op->open(inode, filp);
  }
```
![Figure-10-7](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/d7fca85f-15d7-425d-94de-37463b915dd8)

#Step module_exit:
        * unregister device number unregister_chrdev_region();

        * device file deletion class_destory(), device_destory().

        * Unregister device driver cdev_del();

       

