# Device tree structure

Overview:
  + Its a collection of device nodes
  + A 'device node' or simply called 'node' represents a device. Nodes are oragnized in some systematic way inside device tree file.
  + It has parent , child repleations, and every device tree must have a root node.
  + A node explains itself, that is , reveals its data ans resources using its properties.

During the linux boot process, "Device tree blob" (DTB) file is loaded into memory by U-boot, and a pointer to it is passed to the kernel.

**Syntax**
```
/ {
    node1 {
        a-string-property = "a string";
        a-string-list-property = "first string", "second string";
        A 32 bit interger-property =<100>;
        a-byte-data-property = [0x01 0x02 0x03];
        child-node1 {
            first-child-property;
            second-child-property
            a-string-property = "child string";
        };
        child-node2 {
        };
    };
    node2 {
        an-empty-property;
        a-cell-property = <1 2 3>;
        child-node1 {
        };
    };
};
```

The SOC specific device tree will be given by the vendor in the form of device tree inclusion file(**dtsi**),
and we need to include in our board level device tree.

# Experiment in Yocto

1. Install device tree compiler.
   ```
   sudo apt-get update
   sudo apt-get install device-tree-compiler
   ```

2. Decompile DTB file to get DTS file to make changes in DTS.
   ```
   dtc -I dtb -O dts qemu.dtb -o qemu.dts
   ```

3. After modified, to recompile
   ```
   dtc -I dts -O dtb qemu.dts -o qemu2.dtb
   ```
4. **Important step:** pass the modified dtb file while running qemu.
   ```
    runqemu qemuarm64 nographic slirp qemuparams="-dtb qemu2.dtb"
   ```

# Add new device node in the dts file

1. Change working directory to build-qemu-arm64/workspace/sources/linux-yocto.
2. open "qemu.dts" file (which was extracted using the above command in step 2).
3. Added two new devices in qemu.dts file.

```
	chardrivernode1 {
		compatible = "chardriver";
		device_number = <1>;
		interrupts = <0x0 0x3 0x4>;
	};

	chardrivernode2 {
		compatible = "chardriver";
		device_number = <2>;
		interrupts = <0x0 0x4 0x4>;
	};
```

4. recompile the dts file to create new dtb file.
   ```
   dtc -I dts -O dtb qemu.dts -o qemu2.dtb
   ```

5. run qemu with new dtb file
   ```
   runqemu qemuarm64 nographic slirp qemuparams="-dtb qemu2.dtb"
   ```
