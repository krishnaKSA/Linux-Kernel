# Netwrok drivers

A network interface exchanges packets of information outside world.A network device ask to push incoming packet towrds kernel. The network API looks different than the stuffs we have seen previously with the kernel.

A **Network subsystem** of a kernel is designed to be **protocol independent**. Interaction between a network driver and the 
and kernel properly deals with **one packet at a time.** This enables protocol issues to be hidden neatly from the driver and the physical transmission to be hidden from the protocol.

- octet -> set of eight bits in the networking world.
+ header -> set of octets prepended to a packet as it passed through various layers of the networking subsystems.
     1 example, when application sends a block of data through a TCP socket, the networking subsystem breaks that data up into packets and put a TCP header at the beginning.

# sample setup

  sn0(local0) is connected to snullnet0(remote0)

  sn1(local1) is connected to snullnet1(remote1)
  ```
  LSB is changed in third octet
  snullnet0 192.168.0.0
  snullnet1 192.168.1.0

  The following are possible host numbers to put into /etc/hosts:
  192.168.0.1 local0
  192.168.0.2 remote0
  192.168.1.2 local1
  192.168.1.1 remote1

  ifconfig sn0 local0
  ifconfig sn1 local1
  You may need to add the netmask 255.255.255.0 parameter if the address range chosen is not a class C range.

  ```
  ![download](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/78fa16ac-d7c7-4735-a0de-b64db9d8503e)

# Device registration

Network driver doesn't have device number. Instead, the driver inserts a data structure for each newly detected intercaes into a global list of network devices.

Each interface discribed in "struct net_device item" which is defined in <linux/netdevice.h>
> struct net_device *snull_devs[2];

+ This structure contains kobject, so reference counted and exported via sysfs.
- Must be allocated dynamically
```
struct net_device *alloc_netdev(int sizeof_priv,
                                const char *name,
                                void (*setup)(struct net_device *));
```

+ sizeof_priv is the size of the driver’s private data area
- name is the name of this interface seen by user space
+ setup is a pointer to an initialization function called to set up the rest of the net_device structure.

  ```
  snull_devs[0] = alloc_netdev(sizeof(struct snull_priv), "sn%d",
    snull_init);
snull_devs[1] = alloc_netdev(sizeof(struct snull_priv), "sn%d",
    snull_init);
if (snull_devs[0] = = NULL || snull_devs[1] = = NULL)
    goto out;
    ```

To allocate a network device with the eth%d name argument (%d chooses the next available number), the networking subsystem provides a helper function:

> struct net_device *alloc_etherdev(int sizeof_priv);
This provides its own initialization function (ether_setup)

Once the net_device structure is initialized, we register it with register_netdev like this:
```
for (i = 0; i < 2; i++)
     if ((result = register_netdev(snull_devs[i])))
        printk("snull: error %i registering device \"%s\"\n",
               result, snull_devs[i]->name);
```
Again, don't register until everything is completely initialized!

# Module Unloading
This is fairly typical for a kernel module. It unregisters the interfaces, performs whatever internal cleanup is required, and releases the net_device structure back to the system with:

void snull_cleanup(void)
{
     int i;
     
     for (i = 0; i < 2; i++) {
         if (snull_devs[i]) {
             unregister_netdev(snull_devs[i]);
             snull_teardown_pool(snull_devs[i]);
             free_netdev(snull_devs[i]);
         }
     }
     return;
}
unregister_netdev removes the interface from the system
free_netdev returns the net_device structure to the kernel
