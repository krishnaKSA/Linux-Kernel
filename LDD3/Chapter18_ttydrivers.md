# TTY Driver

TTY devices are typically any serial port sytle device. Example, Serial ports, USB to serial port converters, some modems.

tty virtual devices supports virtual console. These are used for things like longin from a keyboard, network, xterm sessions.

![download](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/d99ce445-1145-44a7-8310-ff895aba2231)

1. **ttycore** is responsible for controlling flow of data through tty device and the format of the data.
1. **tty driver** focuses on handling data to and from hardware.
1. **tty line disciplines** handles data flow control. It can be plugged virtually into any tty device.

There are three different types of tty drivers:

1. console
1. serial port
1. pty

To know what kind of tty drivers are loaded in the kernel and what tty devices are present, look into /proc/tty/drivers.

```
$ cat /proc/tty/drivers

/dev/tty             /dev/tty        5       0 system:/dev/tty
/dev/console         /dev/console    5       1 system:console
/dev/ptmx            /dev/ptmx       5       2 system
/dev/vc/0            /dev/vc/0       4       0 system:vtmaster
rfcomm               /dev/rfcomm   216 0-255 serial
dbc_serial           /dev/ttyDBC   242       0 serial
dbc_serial           /dev/ttyDBC   243       0 serial
ttyprintk            /dev/ttyprintk   5       3 console
max310x              /dev/ttyMAX   204 209-224 serial
serial               /dev/ttyS       4 64-111 serial
pty_slave            /dev/pts      136 0-1048575 pty:slave
pty_master           /dev/ptm      128 0-1048575 pty:master
unknown              /dev/tty        4 1-63 console
```

All tty devices registered and present in the kernel have their own subdrirectory under /sys/class/tty/. Within that subdirectory there is a dev file that contains the major and minor number assigned to that tty device. 

```
$ tree /sys/class/tty

/sys/class/tty
├── console -> ../../devices/virtual/tty/console
├── ptmx -> ../../devices/virtual/tty/ptmx
├── tty -> ../../devices/virtual/tty/tty
├── tty0 -> ../../devices/virtual/tty/tty0
├── tty1 -> ../../devices/virtual/tty/tty1
├── tty10 -> ../../devices/virtual/tty/tty10
├── tty11 -> ../../devices/virtual/tty/tty11
├── tty12 -> ../../devices/virtual/tty/tty12
├── tty13 -> ../../devices/virtual/tty/tty13
├── tty14 -> ../../devices/virtual/tty/tty14
├── tty15 -> ../../devices/virtual/tty/tty15
├── tty16 -> ../../devices/virtual/tty/tty16
├── tty17 -> ../../devices/virtual/tty/tty17
├── tty18 -> ../../devices/virtual/tty/tty18
├── tty19 -> ../../devices/virtual/tty/tty19
├── tty2 -> ../../devices/virtual/tty/tty2
├── tty20 -> ../../devices/virtual/tty/tty20
...

$ tree /sys/class/tty/tty0

/sys/class/tty/tty0
├── active
├── dev
├── power
│   ├── async
│   ├── autosuspend_delay_ms
│   ├── control
│   ├── runtime_active_kids
│   ├── runtime_active_time
│   ├── runtime_enabled
│   ├── runtime_status
│   ├── runtime_suspended_time
│   └── runtime_usage
├── subsystem -> ../../../../class/tty
└── uevent

2 directories, 12 files
```

# Simple tty driver

The main structure is struct tty_driver , it is used to register and unregister a tty driver with the ttycore. Its under <linux/tty_driver.h>

```
struct tty_driver {
	struct kref kref;
	struct cdev **cdevs;
	struct module	*owner;
	const char	*driver_name;
	const char	*name;
	int	name_base;
	int	major;
	int	minor_start;
	unsigned int	num;
	short	type;
	short	subtype;
	struct ktermios init_termios;
	unsigned long	flags;
	struct proc_dir_entry *proc_entry;
	struct tty_driver *other;

	/*
	 * Pointer to the tty data structures
	 */
	struct tty_struct **ttys;
	struct tty_port **ports;
	struct ktermios **termios;
	void *driver_state;

	/*
	 * Driver methods
	 */

	const struct tty_operations *ops;
	struct list_head tty_drivers;
} __randomize_layout;
```

To create stry tty_driver, we need to call alloc_tty_driver with number of devices this driver is going to support.

```
/* allocate the tty driver */
ttydriver = tty_alloc_driver(1, TTY_DRIVER_RESET_TERMIOS |TTY_DRIVER_REAL_RAW |TTY_DRIVER_UNNUMBERED_NODE);
if (!ttydriver)
    return -ENOMEM;
```

struct tty_operations serial_ops=
{
}

