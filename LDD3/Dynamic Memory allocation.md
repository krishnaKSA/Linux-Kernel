kmalloc function includes from <linux/slab.h>
```
char *buf;
buf = kmalloc(BUF_SIZE, GFP_ATOMIC);
if (!buf)
        /* error allocting memory ! */
kfree(buf);
```

```
FLAG         Description
__GFP_WAIT - The allocator can sleep.
__GFP_HIGH - The allocator can access emergency pools.
__GFP_IO - The allocator can start disk I/O.
__GFP_FS - The allocator can start filesystem I/O.r
__GFP_COLD - The allocator should use cache cold pages.
__GFP_NOWARN - The allocator will not print failure warnings.
__GFP_REPEAT - The allocator will repeat the allocation if it fails, but the allocation can potentially fail.
__GFP_NOFAIL-  The allocator will indefinitely repeat the allocation. The allocation cannot fail.
__GFP_NORETRY-The allocator will never retry if the allocation fails.
__GFP_NO_GROW - Used internally by the slab layer.
__GFP_COMP - Add compound page metadata. Used internally by the hugetlb code.
```

```
GFP_ATOMIC
The allocation is high priority and must not sleep. This is the flag to use in interrupt handlers, in bottom halves, while holding a spinlock, and in other situations where you cannot sleep.

GFP_NOIO
This allocation can block, but must not initiate disk I/O. This is the flag to use in block I/O code when you cannot cause more disk I/O, which might lead to some unpleasant recursion.

GFP_NOFS
This allocation can block and can initiate disk I/O, if it must, but will not initiate a filesystem operation. This is the flag to use in filesystem code when you cannot start another filesystem operation.

GFP_KERNEL
This is a normal allocation and might block. This is the flag to use in process context code when it is safe to sleep.
The kernel will do whatever it has to in order to obtain the memory requested by the caller. This flag should be your first choice.

GFP_USER
This is a normal allocation and might block. This flag is used to allocate memory for user-space processes.

GFP_HIGHUSER
This is an allocation from ZONE_HIGHMEM and might block. This flag is used to allocate memory for user-space processes.

GFP_DMA
This is an allocation from ZONE_DMA. Device drivers that need DMA-able memory use this flag, usually in combination with one of the above.
```
```
GFP_ATOMIC - __GFP_HIGH
GFP_NOIO - __GFP_WAIT
GFP_NOFS -(__GFP_WAIT | __GFP_IO)
GFP_KERNEL -(__GFP_WAIT | __GFP_IO | __GFP_FS)
GFP_USER -(__GFP_WAIT | __GFP_IO | __GFP_FS)
GFP_HIGHUSER -(__GFP_WAIT | __GFP_IO | __GFP_FS | __GFP_HIGHMEM)
GFP_DMA __GFP_DMA
```
