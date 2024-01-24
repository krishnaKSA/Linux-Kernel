
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
        
        * Implemenet the drivers file operation methods for open, read, write, llseek, etc.,

Step module_exit:
        * unregister device number unregister_chrdev_region();

        * device file deletion class_destory(), device_destory().

        * Unregister device driver cdev_del();

       

