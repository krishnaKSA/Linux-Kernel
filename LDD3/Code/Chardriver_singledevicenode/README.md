TESTED IN QEMU
This char driver creates single device node 

How to test it?

kernel module name : chardriver

Userapplication name: chardriver_userapp

1. Mount all the necessary file systems
2. modprobe kernel module
3. launched user app

# load kernel module
```
 # mkdir proc
 # mkdir dev
 # mount -t sysfs ysfs /sys
 # mount -t proc proc /proc
 # mount -t devtmpfs devtmpfs /dev
 # 
 # modprobe char-driver
[   27.642374] char_driver: loading out-of-tree module taints kernel.
[   27.654220] Module init called 
[   27.654525] Module init: device number created MAJOR=249  
[   27.654905] Module_init: Device registered with vfs . Device number = 249
[   27.655382] Module init: sysfs class created
[   27.655889] Module_init: Device created with vfs . Device number = chdriver-0
```

 # launch user application
 ```
 # **chardriver_userapp**

kernel logs -> [   35.967115] chdriver_read: read requested for 20 bytes , f_pos =0 
kernel logs -> [   35.967761] chdriver_read:: read completed for bytes 20,  fpos updated to 20

userapp logs -> userapp: read successful count =20

kernel logs ->[   35.974612] chdriver_write:: write requested for bytes 30,  f_pos updated to 20
kernel logs ->[   35.975061] chdriver_write:: write completed for bytes 30,  fpos updated to 50

userapp logs -> userapp: write successful count = 30

kernel logs ->[   35.975842] chdriver_read: read requested for 23 bytes , f_pos =50 
kernel logs ->[   35.976187] chdriver_read:: read completed for bytes 23,  fpos updated to 73

userapp logs -> userapp: read successful = 23

on close(fd):
[   35.976914] chdriver_release: called

```

# Device files
```
/ # cd sys/class/chdriver/chdriver-0/
/sys/devices/virtual/chdriver/chdriver-0 # 
/sys/devices/virtual/chdriver/chdriver-0 # 
/sys/devices/virtual/chdriver/chdriver-0 # ls -al
drwxr-xr-x    3 0        0                0 Jan 25 17:28 .
drwxr-xr-x    3 0        0                0 Jan 25 17:28 ..
-r--r--r--    1 0        0             4096 Jan 25 17:28 dev
drwxr-xr-x    2 0        0                0 Jan 25 17:28 power
lrwxrwxrwx    1 0        0                0 Jan 25 17:28 subsystem -> ../../../../class/chdriver
-rw-r--r--    1 0        0             4096 Jan 25 17:28 uevent

/sys/devices/virtual/chdriver/chdriver-0 # cat dev 
249:0

/ # cd dev/
/dev # 
/dev # ls
autofs         ram15          tty18          tty39          tty6
btrfs-control  ram2           tty19          tty4           tty60
bus            ram3           tty2           tty40          tty61
chdriver-0     ram4           tty20          tty41          tty62
```

# unload kernel module

```
/ # rmmod char-driver
[  797.148580] char_driver_exit_function completed 
/ # 

```
