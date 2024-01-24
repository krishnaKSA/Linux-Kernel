# Char Driver

Example char driver:
This in the digram, char driver is in kernel space, when the user application wants to commuincate with char driver , it invokes system call.

When the user application calls "**open("dev/rtc",...)** system call, it should connect to the **Open** implementation in the kernel module.

How to establish such conenction from user space to kernel space?
Its taken care by **VFS (Virtual file system)**. That means , device driver should get register to VFS by VFS kernal API's.

![Figure-1-16](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/02bbe2be-c480-44fc-84dc-3d3bf5cf38f5)


# Device number

Driver creates device file interface in the user space for user level programs to communicate with driver.

To establish the connection from user app system call to driver methods, kernel assigns number to the driver .

Device file -> /dev/rtc0 
for example, 4 is the number of device driver. device file /dev/rtc0/ -> 4:0. 0 is the device instance. 
for example, there could be four rtc devices files /dev/rtc0 /dev/rtc1, /dev/rtc2, /dev/rtc3 , all are handled by the same driver 4.![download](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/2294372b-38bd-4f2f-b746-1e627a7dfb43)

4:0 is called as **Device number**.
4 is called **Major number**.
0 is called **Minor number**. (Device instances)

When the user application calls open("/dev/rtc4.0") system calls, VFS will first handle this request.
VFS gets device number , and compare it with its driver registration list. That means, device driver should get resister to VFS with this number.
VFS will select the correct device driver from its list based on the number, and forwards the user request to the driver.
Device driver uses minor number to distingush on which device file read and write opeartions are issued.



