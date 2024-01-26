# Commuincating with hardware:

**I/O ports and I/O memory:**

Every single peripheral device is controlled by reading , and writing of its register. Device has many registers which are accessed consecutive in memory space or I/O address space.

At the hardware level both the memory region and I/O region are same. Both are accessed by electic signal on the address bus and control bus, and by reading and writing from data bus.

**Io registers and conventional memory:**
Someone accessing I/O register must be careful to avoid trick by CPU optimization that can modify expected I/O behavior. Compiler can cache the data into CPU register with out writing them to the actual memory. Both read and write can opearte on cache memory without touching physical memory RAM. Thus, a driver must ensure that no caching is performed and no read or write reordering takes place when accessing registers.

To fix this issue, we place a memory barrier between opeartions that need to be visible to the hardware in a particular order.

```
#include <linux/kernel.h>
void barrier(void)
    /*This function tells the compiler to insert a memory barrier but has no effect on
    the hardware. Compiled code stores to memory all values that are currently
    modified and resident in CPU registers, and rereads them later when they are
    needed. A call to barrier prevents compiler optimizations across the barrier but
    leaves the hardware free to do its own reordering. */
    
#include <asm/system.h>
void rmb(void);
void read_barrier_depends(void);
void wmb(void);
void mb(void);
    /*These functions insert hardware memory barriers in the compiled instruction
    flow. An rmb (read memory barrier) guarantees that any reads appearing before the 
    barrier are completed prior to the execution of any subsequent read. wmb 
    guarantees ordering in write operations, and the mb instruction guarantees both. 
    Each of these functions is a superset of barrier. */
    
    /*read_barrier_depends is a special, weaker form of read barrier. rmb prevents the 
    reordering of all reads across the barrier, read_barrier_depends blocks
    only the reordering of reads that depend on data from other reads. You should 
    stick to using rmb. */
    
void smp_rmb(void);
void smp_read_barrier_depends(void);
void smp_wmb(void);
void smp_mb(void);
    /*These insert hardware barriers only when the kernel is compiled for SMP systems. 
    Otherwise, they all expand to a simple barrier call.*/
    
//And a typical use of memory barriers in a device driver may have this general form:
writel(dev->registers.addr, io_destination_address);
writel(dev->registers.size, io_size);
writel(dev->registers.operation, DEV_READ);
wmb( );
writel(dev->registers.control, DEV_GO);
Memory buses slow down performance in general, so only use them when you need to. The kernel provides a few useful combinations of variable assignment and memory barrier specification in <asm/system.h> as shown:

#define set_mb(var, value) do {var = value; mb( );} while 0
#define set_wmb(var, value) do {var = value; wmb( );} while 0
#define set_rmb(var, value) do {var = value; rmb( );} while 0
```

**I/O ports**

We have to get the access to the I/O ports before using them. The kernel has a registration interface to allow your driver to claim the port it needs. The core function in that interface is request_region:
```
#include <linux/ioport.h>

struct resource *request_region(unsigned long first, unsigned long n,
                                const char *name);

Tell the kernel to make use of nports
Start with the port first
name is the name of your device
Return is non-NULL if allocation succeeds
If you get a return of NULL, you will not be able to access the ports
Ports show up in /proc/ioports

To release
void release_region(unsigned long start, unsigned long n);
```

```
unsigned inb(unsigned port);
void outb(unsigned char byte, unsigned port);
    /*Read or write byte ports (eight bits wide). The port argument is defined as
    unsigned long for some platforms and unsigned short for others. The return
    type of inb is also different across architectures.*/
    
unsigned inw(unsigned port);
void outw(unsigned short word, unsigned port);
    /*These functions access 16-bit ports (one word wide); they are not available 
    when compiling for the S390 platform, which supports only byte I/O. */
    
unsigned inl(unsigned port);
void outl(unsigned longword, unsigned port);
    /*These functions access 32-bit ports. longword is declared as either unsigned 
    long or unsigned int, according to the platform. Like word I/O, “long” I/O is 
    not available on S390.*/
```

**I/O Port Access from User Space:**
The functions mentioned just before this are mainly meant to be used by device drivers, but they can also be sued from user space. The GNU C library defines them in <sys/io.h>. The following conditions should apply to use them in user space:

- The program must be compiled with the -O option to force expansion of inline functions
+ The ioperm or iopl system calls must be used to get permission to perform I/O operations on ports. ioperm gets permission for individual ports, while iopl gets permission for the entire I/O space.
- The program must run as root to invoke ioperm or iopl. Alternatively, one of its ancestors must have gained port access running as root.

String operation:
```
void insb(unsigned port, void *addr, unsigned long count);
void outsb(unsigned port, void *addr, unsigned long count);
    /*Read or write count bytes starting at the memory address addr. Data is read 
    from or written to the single port port.*/
    
void insw(unsigned port, void *addr, unsigned long count);
void outsw(unsigned port, void *addr, unsigned long count);
    //Read or write 16-bit values to a single 16-bit port.
    
void insl(unsigned port, void *addr, unsigned long count);
void outsl(unsigned port, void *addr, unsigned long count);
    //Read or write 32-bit values to a single 32-bit port.
```
Pausing I/O:

Some platforms have problems with timing between the processor and peripheral device. In these situations, a small delay can be inserted after each I/O instruction if another I/O instruction immediately follows. On x86, this is done by performing an out b instruction to port 0x80, a normally unused port. Pausing functions are exactly like all of the ones listed above, but they have a _p added to the end of them (example: outb_p instead of outb).

Platform Dependencies:

Here are some more relevant details on the platforms that I am most concerned with:

x86_64
    The architecture supports all the functions described in this chapter. Port 
    numbers are of type unsigned short.
    
ARM
    Ports are memory-mapped, and all functions are supported. String functions are
    implemented in C. Ports are of type unsigned int.

**An Overview of the Parallel Port**
![800px-Parallel_computer_printer_port](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/d68c218e-6422-47e8-b18a-1eb985304688)


The parallel interface, in its minimal configuration, is made up of three 8-bit ports. The PC standard starts the I/O ports for the first parallel interface at 0x378 and for the second at 0x278.

0x378 is a bidirectional data register. It connects directly to pins 2–9 on the physical connector
0x278 is a read-only status register. 
The third port is an output-only control register which mainly controls whether interrupts are enabled or not
All ports are on the 0-5V TTL logic level

**Using I/O memory:**

The main mechanism is used to communicate with device is memory-mapped register and device memory.Both are called I/O memory.
I/O memory is a region of RAM-like locations that the device makes available to the processor over the bus. It can be used for things like holding video data, holding ethernet packets, and implementing registers that behave like I/O ports.

I/O memory may or may not be accessed through page tables. When access does pass through page tables, the kernel must first arrange for the physical address to be visible from your driver. This usually means that you must call ioremap before doing any I/O operations. If no page tables are needed, I/O memory locations look like I/O ports. You can then just read and write to them using proper wrapper functions.

Whether or not ioremap is required to access I/O memory, direct use of pointers to I/O memory is discouraged. The kernel provides wrapper functions used to access I/O memory that is safe on all platforms and optimized away whenever straight pointer dereferencing can perform the operation.

**I/O Memory Allocation and Mapping:**

```
I/O memory regions need to be allocated prior to use. The interface for allocation of memory regions is described in <linux/ioport.h> and has prototype:

struct resource *request_mem_region(unsigned long start, unsigned long len,
                                    char *name);
Allocates a memory region of len bytes
Allocates starting at the bit start
On success, a non-NULL pointer is returned.
On failure, the return value is NULL.
All I/O memory allocations are listed in /proc/iomem.

Memory regions should be free when no longer needed with:

void release_mem_region(unsigned long start, unsigned long len);
There is also an old, now outdated method to check I/O memory region availability with:

int check_mem_region(unsigned long start, unsigned long len);
It still shows up sometimes, so it is included here for completeness.

Next, virtual addresses must be assigned to I/O memory regions with ioremap. Once equipped with ioremap and iounmap, a device driver can access any I/O memory address whether or not it is directly mapped to virtual address space. Here are the prototypes for ioremp:

#include <asm/io.h>

void *ioremap(unsigned long phys_addr, unsigned long size);
void *ioremap_nocache(unsigned long phys_addr, unsigned long size);
void iounmap(void * addr);
The nocache version is often just identical to ioremap. It was meant to be useful if some control registers were in such an area that write combining or read caching was not desirable. This is rare and often not used.
```

**Accessing I/O Memory:**
```
On some platforms, you may get away with using the return value from ioremap as a pointer. This is not good though. Instead, a more portable version with functions has been designed as follows:

To read from I/O memory, use one of the following:

unsigned int ioread8(void *addr);
unsigned int ioread16(void *addr);
unsigned int ioread32(void *addr);
Where addr is an address obtained from ioremap. The return value is what was read from the given I/O memory.

To write to I/O memory, use one of the following:

void iowrite8(u8 value, void *addr);
void iowrite16(u16 value, void *addr);
void iowrite32(u32 value, void *addr);
For reading or writing multiple values, use the following:

void ioread8_rep(void *addr, void *buf, unsigned long count);
void ioread16_rep(void *addr, void *buf, unsigned long count);
void ioread32_rep(void *addr, void *buf, unsigned long count);
void iowrite8_rep(void *addr, const void *buf, unsigned long count);
void iowrite16_rep(void *addr, const void *buf, unsigned long count);
void iowrite32_rep(void *addr, const void *buf, unsigned long count);
These functions read or write count values from the given buf to the given addr
count is expressed in the size of the data being written
Ex: ioread32_rep reads count 32-bit values starting at buf
If you need to operate on a block of memory instead, use one of the following:

void memset_io(void *addr, u8 value, unsigned int count);
void memcpy_fromio(void *dest, void *source, unsigned int count);
void memcpy_toio(void *dest, void *source, unsigned int count);
These functions behave like their C library analogs
There are some older read/write functions in legacy kernel code that you should watch out for. Some 64-bit platforms also offer readq and writeq, for quad-word (8-byte) memory operations on the PCI bus.
```

**Ports as I/O Memory**
```
Some hardware has an interesting feature: some versions use I/O ports, while others use I/O memory. This seems really confusing to me as to why you would do both. The registers exported to the processor are the same in both cases, but the access method is different (this just sounds like a bad time). To minimize the difference between these two access methods, the kernel provides a function called ioport_map with prototype:

void *ioport_map(unsigned long port, unsigned int count);
It remaps count I/O ports and makes them appear to be I/O memory
From there on the driver can use ioread8 and related functions on the returned addresses
This makes it forget that it is using I/O ports at all
The mapping should be undone when no longer needed with:

void ioport_unmap(void *addr);
Note: I/O ports must still be allocated with request_region before they can be remapped in this way.
```
