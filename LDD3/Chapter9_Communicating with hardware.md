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
https://en.wikipedia.org/wiki/Parallel_port#/media/File:Parallel_computer_printer_port.jpg

The parallel interface, in its minimal configuration, is made up of three 8-bit ports. The PC standard starts the I/O ports for the first parallel interface at 0x378 and for the second at 0x278.

0x378 is a bidirectional data register. It connects directly to pins 2–9 on the physical connector
0x278 is a read-only status register. 
The third port is an output-only control register which mainly controls whether interrupts are enabled or not
All ports are on the 0-5V TTL logic level

