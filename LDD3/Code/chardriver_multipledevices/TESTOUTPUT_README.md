
TESTED IN QEMU This char driver creates multiple device nodes. Currenlty set MAX devices as 2.

How to test it?

kernel module name : chardriver_multipledevices

Userapplication name: chardrivermuldevice_userapp

Mount all the necessary file systems

modprobe kernel module

launched user app

# Load kernel module
```
/ # modprobe chardriver_multipledevices
[   53.089302] chardriver_multipledevices: loading out-of-tree module taints kernel.
[   53.101736] Module init called 
[   53.102079] Module init: device number created MAJOR=249  
[   53.102583] Module init: sysfs class created
[   53.102647] Module_init: Device registered with vfs . Device number = 249, index = 0
[   53.104254] Module_init: Device created with vfs . Device number = 249
[   53.105295] Module_init: Device registered with vfs . Device number = 249, index = 1
[   53.106354] Module_init: Device created with vfs . Device number = 249
```
# launch user application
```
/ # chardrivermuldevice_userapp
driver logs -> [   64.746106] Device file 249:0 was opened by the user 
driver logs ->[   64.746637] chdriver_read: read requested for 24 bytes , f_pos =0 
driver logs ->[   64.747113] chdriver_read:: read completed for bytes 24,  fpos updated to 24

userapp: read successful count =24 

driver logs ->[   64.753825] chdriver_write:: write requested for bytes 23,  f_pos updated to 24
driver logs ->[   64.754276] chdriver_write:: write completed for bytes 23,  fpos updated to 47

userapp: write successful 

driver logs ->[   64.755434] Device file 249:1 was opened by the user 
driver logs ->[   64.755764] chdriver_read: read requested for 45 bytes , f_pos =0 
driver logs ->[   64.756307] chdriver_read:: read completed for bytes 45,  fpos updated to 45

userapp: read successful =45

driver logs ->[   64.757845] chdriver_write:: write requested for bytes 23,  f_pos updated to 45
driver logs ->[   64.758204] chdriver_write:: write completed for bytes 23,  fpos updated to 68

userapp: write successful count=23

driver logs -> [   64.758995] chdriver_release: called 
driver logs ->[   64.759733] chdriver_release: called 
```
# device files
```
/ # cd sys/class/chdriver/chdriver-

/sys/class/chdriver # ls -al
drwxr-xr-x    2 0        0                0 Jan 25 17:48 .
drwxr-xr-x   31 0        0                0 Jan 25 17:48 ..
lrwxrwxrwx    1 0        0                0 Jan 25 17:48 chdriver-1 -> ../../devices/virtual/chdriver/chdriver-1
lrwxrwxrwx    1 0        0                0 Jan 25 17:48 chdriver-2 -> ../../devices/virtual/chdriver/chdriver-2

/sys/class/chdriver # cat chdriver-1/dev 
249:0
/sys/class/chdriver # cat chdriver-2/dev 
249:1

/ # cd dev/
/dev # 
/dev # ls -al | grep -i "chdriver"
crw-------    1 0        0         249,   0 Jan 25 17:48 chdriver-1
crw-------    1 0        0         249,   1 Jan 25 17:48 chdriver-2
```

# remove module
/ # rmmod chardriver_multipledevices
chdriver:module removed successfully
