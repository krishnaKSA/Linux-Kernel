Built inbuilt char driver kernel module.

# STEP 1: Created inbuilt char driver module. 
Follwed steps in Experiment on Kconfig.md
+ Created source file under yocto/poky/build-qemu-arm64/workspace/sources/linux-yocto/driver/char/ folder.
+ Created Kconfig , and included object in Makefile.
+ Enabled module through menuconfig.


# STEP 2: Created device node in qemu.dts file.
Follwed steps as I have mentioned in the Experiment on device tree.md. 
+ Added two new devices in qemu.dts file.
+ Added interrupt property.
+ Added device number property which used while creating device node.
+ Added compatible property
+ Build the new dtb file.

# STEP 3: Build kernel module
+ Created kernel module which is compatible with the device node added in the qts file.
+ In the probe function, registered for the interrupt. Read interrupt number from dts file.
+ To communicate from user app, and transfters data bewtween this driver and user app, created device files
  which will be popluated in /dev/ fs.

# STEP 4: Simualted HW interrupt in qemu. 
+ Enabled devmem in yocto/poky/meta/recipes-core/busybox/busybox/defconfig file
+ Issued "devmem 0x8000104 w 0x0000002E" to enable interrupt.
+ Issued "devmem 0x8000204 w 0x00000008" to trigger interrupt at 35th bit. 
  
