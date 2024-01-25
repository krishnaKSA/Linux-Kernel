
steps to build a minimal Yocto Linux image for a 64-bit Arm target and run it on QEMU  .

Build minimal Yocto Linux image for 64-bit Arm target
Poky is a reference distribution of the Yocto Project. It is a great starting point to build your own custom distribution as it contains both the build system and the the baseline functional distribution. Along with containing recipes for real target boards, it also contains the recipes for building the image for example 64-bit Arm machines supported in QEMU. The example 64-bit machine emulated by QEMU does not emulate any particular board but is a great starting point to learn and try the basics of running this distribution.

The first step is to install the packages required to build and run Yocto
```
sudo apt update
sudo apt-get install -y gawk wget git-core diffstat unzip texinfo build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint xterm python3-subunit mesa-common-dev
```

Now download the Poky reference distribution and checkout the branch/tag you wish to build. You will build yocto-4.0.6 in this example.

```
git clone git://git.yoctoproject.org/poky
cd poky
git checkout tags/yocto-4.0.6 -b yocto-4.0.6-local
```

Next source the script as shown below to initialize your build environment for your 64-bit Arm example machine QEMU target
```
source oe-init-build-env build-qemu-arm64
```

You will now be in the build-qemu-arm64 directory which is your build directory and where the images for your target are built. As the output from running the command above indicates, you will now need to select the target hardware MACHINE in the conf/local.conf file. To do this, run sed to uncomment MACHINE ?= "qemuarm64" in conf/local.conf file.
```
sed -i '/qemuarm64/s/^#//g' conf/local.conf
```
With the right machine now selected, proceed to building the minimal core image for your target.
```
bitbake core-image-minimal
```

Depending on your machine, this build step can take an hour or more to complete.

After the build is complete, the images are in the build-qemu-arm64/tmp/deploy/images/qemuarm64 directory.

# Run the image on the 64-bit Arm QEMU target
QEMU is installed on your machine as part of cloning the Poky repository and sourcing the environment script.
You can now run the command below to launch run the image you built on the 64-bit Arm Qemu target

```
runqemu qemuarm64 nographic slirp
```
