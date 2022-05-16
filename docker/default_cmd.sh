#!/bin/bash
set -ex

fail() {
    echo $1 >&2
    exit 1
}

SRC=/home/student/src
if [ ! -f $SRC/linux/Makefile ] ; then
    fail "Mount a linux tree volume under $SRC/linux"
fi
cd $SRC/linux

if [ ! -f .config ] ; then
    echo ".config not found, using versatile template"
    make ARCH=arm versatile_defconfig
    # Enable debug symbols and devtmpfs support
    sed -i \
        -e 's/.*CONFIG_DEBUG_INFO.*/CONFIG_DEBUG_INFO=y/' \
        -e 's/.*CONFIG_DEVTMPFS.*/CONFIG_DEVTMPFS=y/' .config
    make ARCH=arm olddefconfig
fi
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- all

if ! cd $SRC/initramfs ; then
    fail "Mount a volume with initramfs source files under $SRC/initramfs"
fi
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

BOOT=$SRC/linux/arch/arm/boot
qemu-system-arm -M versatilepb -m 128M -nographic -kernel $BOOT/zImage -dtb $BOOT/dts/versatile-pb.dtb -initrd build/rootfs.gz -append "root=/dev/ram"
