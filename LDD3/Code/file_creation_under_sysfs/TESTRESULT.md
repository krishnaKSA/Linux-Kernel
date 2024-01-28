Tested in qemu. 

1. Load kernel module "sysfs_file.ko"
2. read and write data into /sys/kernel/sysfs_buf/buffer

   ```
  /sys/kernel/sysfs_buf # ls -al
  drwxr-xr-x    2 0        0                0 Jan 28 15:48 .
  drwxr-xr-x   13 0        0                0 Jan 28 15:26 ..
  -rw-rw----    1 0        0             4096 Jan 28 15:49 data_buffer
  
  /sys/kernel/sysfs_buf # cat data_buffer  
  /sys/kernel/sysfs_buf # echo "testdata" > data_buffer 
  
  /sys/kernel/sysfs_buf # cat data_buffer 
  testdata
  
  ```
