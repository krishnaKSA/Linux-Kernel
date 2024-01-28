
Kconfig : selection based configuration system. Its commonly used to select build time options and to enable or disable features.

If we have to list our kernel module selection in kernel menuconfig, then we need to create and use Kconfig.

When we do menuconfig, linux configuration will come in the forms of menu, and from there we can select or deselect kernel modules.

# Experiment in yocto.

**STEP 1: Write Kconfig for our kernel module**

1. Check out the kernel source files:
   ```
   devtool modify linux-yocto (in /yocto/poky)
   ```
3. Change the work directory.
   ```
   yocto/poky/build-qemu-arm64/workspace/sources/linux-yocto
   ```
5. Created my kernel module source file inside /drivers/char/ folder.
6. Then, modified **Kconfig** file inside /drivers/char/ folder to include my kernel module.
   I have given config name as "SAMPLE_CHARDRIVER" and default n means its disabled by default.
     ```
      config SAMPLE_CHARDRIVER
    	bool "enable sample driver"
    	default n
     ```
7. Next in the same directory, add my kernel module in the **Makefile**.
   ```
    obj-$(CONFIG_SAMPLE_CHARDRIVER)	+=samplechardriver.o
   ```
    This step states that, if the config for SAMPLE_CHARDRIVER is enabled , then include that module while compiling.

**STEP 2: HOW TO ENABLE KERNEL MODULE**

1. Run the below command in yocto/poky/build-qemu-arm64/workspace/sources/linux-yocto/ folder.
   > bitbake linux-yocto -c menuconfig

2. The below GUI will be opened. Enter / and enter the config to search.
   
   ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/d808d639-77eb-4d10-a047-e47ec80f59b6)

   ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/4acf6a85-72bd-4fcc-a2ac-a01416cb1aee)

   ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/9d01014f-94dd-4634-9c27-18ad6c3fe2c9)


4. As per the config, its in disable state. **Enter 1 to enable it**.    
   ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/45f8404d-2208-4939-921f-98f36bec326b)
   
6. Then **enter "y" to enable** it.
    ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/5699811e-f0eb-4880-83a6-85d5df760369)

7. Save the configuration.
   
   ![image](https://github.com/krishnaKSA/Linux-Kernel/assets/60934956/bdf2c5c3-ddd5-4cec-b58e-3b9ca3de1dda)

9. Now, trigger the build.
   ```
   bitbake core-image-minimal
   ```

10. Run the image on qemu.
   ```
   runqemu qemuarm64 nographic slirp
   ```

   
