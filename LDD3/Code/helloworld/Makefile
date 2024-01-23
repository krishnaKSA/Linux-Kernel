#architecture ARM V8
ARCH=arm64

#Kernel directory
KERNEL_DIR=/data/home/krkrishnasam/kmods/kernel/linux-5.4.267

#this module directory
THIS_MODULE_DIR=${shell pwd}

#Makefile sysmbol used by kernel build system to determine which module should built
obj-m += helloworld.o

all:
	CROSS_COMPILE=aarch64-linux-gnu- ARCH=arm64 make -C ${KERNEL_DIR} M=${THIS_MODULE_DIR} modules -j32

clean: 
	rm helloworld.ko helloworld.o
