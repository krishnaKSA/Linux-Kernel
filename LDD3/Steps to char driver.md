
Step 1: connection establish between device file access and driver.
        * Create device number
        * Create device files
        * Make a char device registation with the VFS(CDEV_ADD)
        * Implemenet the drivers file operation methods for open, read, write, llseek, etc.,
