Tested in QEMU. Hardware interrupt simulated in qemu.

+ run this command in yocto build system, > runqemu qemuarm64 nographic slirp qemuparams="-dtb qemu2.dtb"
+ samplechardriver kernel module is inbuilt.

+ After boot, you can see these logs
+ Registered as platform driver.
+ In the probe function, read the prop value for interrupt from device node, and registered for interrupt.
+ Device files created /dev/samchdriver-1, /dev/samchdriver-2.
  ```
  / # dmesg | grep -i "sample"
  [    2.803796] SAMPLECHDRIVER:: INIT FUNCTION
  [    2.804873] SAMPLECHDRIVER:: char device number major=249, minor=0
  [    2.805813] SAMPLECHARDRIVER chardrivernode1: sample_chardev_probe called 
  [    2.806254] SAMPLECHARDRIVER chardrivernode1: sample_chardev_probe prop_dev_num  = 1
  [    2.806943] SAMPLECHARDRIVER chardrivernode1: sample_chardev_probe:: IRQ number = 49 
  [    2.809406] SAMPLECHARDRIVER chardrivernode1: interrupt request succeeded 
  [    2.810781] SAMPLECHARDRIVER chardrivernode1: Module_init: Device created with vfs samchdriver-1 
  [    2.811689] SAMPLECHARDRIVER chardrivernode2: sample_chardev_probe called 
  [    2.812686] SAMPLECHARDRIVER chardrivernode2: sample_chardev_probe prop_dev_num  = 2
  [    2.813220] SAMPLECHARDRIVER chardrivernode2: sample_chardev_probe:: IRQ number = 50 
  [    2.813767] SAMPLECHARDRIVER chardrivernode2: interrupt request succeeded 
  [    2.814908] SAMPLECHARDRIVER chardrivernode2: Module_init: Device created with vfs samchdriver-2 
  [    2.815556] SAMPLECHDRIVER:: init completed. ret=0 
  ```
  ```
  /sys/bus/platform/drivers/SAMPLECHARDRIVER # ls -al
  drwxr-xr-x    2 0        0                0 Jan 28 15:26 .
  drwxr-xr-x   19 0        0                0 Jan 28 15:26 ..
  --w-------    1 0        0             4096 Jan 28 15:27 bind
  lrwxrwxrwx    1 0        0                0 Jan 28 15:27 chardrivernode1 -> ../../../../devices/platform/chardrivernode1
  lrwxrwxrwx    1 0        0                0 Jan 28 15:27 chardrivernode2 -> ../../../../devices/platform/chardrivernode2
  --w-------    1 0        0             4096 Jan 28 15:27 uevent
  --w-------    1 0        0             4096 Jan 28 15:27 unbind
  ```

  ```
  /sys/class/samplechdrv # ls -al
  drwxr-xr-x    2 0        0                0 Jan 28 15:27 .
  drwxr-xr-x   31 0        0                0 Jan 28 15:26 ..
  lrwxrwxrwx    1 0        0                0 Jan 28 15:27 samchdriver-1 -> ../../devices/virtual/samplechdrv/samchdriver-1
  lrwxrwxrwx    1 0        0                0 Jan 28 15:27 samchdriver-2 -> ../../devices/virtual/samplechdrv/samchdriver-2
  ```
  ```
  /dev # ls -al|grep -i sam
  crw-------    1 0        0         249,   1 Jan  1  1970 samchdriver-1
  crw-------    1 0        0         249,   2 Jan  1  1970 samchdriver-2
  ```
**Launch userapp**
```
 # chardriver_userapp &
/ # [ 1016.190002] Sample_chdriver_open called 
[ 1016.190393] access is ok
[ 1016.191120] Sample_chdriver_read called 
[ 1016.191386] chdriver_read: Sync bit is not set. wait for it 
```
**Read will not happen until the sync flag is set on the driver. Sync flag is set once interrupt received**

**Simulate interrupt in qemu**
+ To enable interrupt at 35th bit
  > devmem 0x8000104 w 0x0000002E
+ To trigger interrupt at 35th bit
  > devmem 0x8000204 w 0x00000008

After receiving interrupts, read and write requested are processed . 
```
/ # devmem 0x8000204 w 0x00000008
[ 1387.589321] sample_chardev: interrupt_handler called for int no 49
/ # userapp: read successful count =20 
userapp: write successful count = 30
[ 1387.598919] Sample_chdriver_read called 
userapp: read successful = 23
[ 1387.604857] Sample_chdriver_release called 
```

**Interrupt received by device chardrivernode1**

![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/7b7bf91c-24f3-45d0-87ea-a08216b030c1)































  
