
# load kernel module
```
/ # modprobe ioctl_driver
[  788.077237] IOCTL: init_ioctl_driver: completed 
```
# launch user application
```
/ # ioctl_userapp

[  795.703120] IOCTL: ioctl_open  called 
[  795.703706] IOCTL: ioctl_handler IOCTLREAD called 
[  795.704191] iOCTL:ioctl_handler: read data sucessfully. budrate =100

IOCTL:USRAPP: read completed . baudrate = 100

[  795.711066] IOCTL: ioctl_handler IOCTLWRITE called 
[  795.711482] iOCTL:ioctl_handler: write data sucessfully. budrate =500

IOCTL:USRAPP: write completed . baudrate = 500

[  795.712383] IOCTL: ioctl_release  called 
```

# device node
```
/sys/devices/virtual/ioctldriver/ioctldriver-0 # ls
dev        power      subsystem  uevent

/sys/devices/virtual/ioctldriver/ioctldriver-0 # cat dev 
249:0


/dev # ls -al |grep -i ioctl
crw-------    1 0        0         249,   0 Jan 25 18:00 ioctldriver-0
```
# unload kernel module
```
/ # rmmod ioctl_driver
[  865.899430] IOCTL: exit_ioctl_driver completed
```
