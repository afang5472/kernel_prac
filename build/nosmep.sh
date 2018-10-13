#!/bin/bash 

#initialize dirs
rm ./busybox/_install/*.ko
rm ./rootfs.img
mkdir -p ./busybox/_install/mods
mkdir -p ./busybox/_install/exps
rm ./busybox/_install/mods/*
rm ./busybox/_install/exps/*

#copy modules
cp ./mod/stkove.ko ./busybox/_install/mods/stkove.ko
cp ./ioctl_mod/ioctl.ko ./busybox/_install/mods/ioctl.ko
cp ./heap/heap.ko ./busybox/_install/mods/heap.ko

#copy exps
cp ./ioctl_exp/test ./busybox/_install/exps/ioctl_exp
cp ./heap_exp/heap ./busybox/_install/exps/heap_exp
cp ./rop/rop ./busybox/_install/exps/rop
cp ./heap_exp/heap_cred ./busybox/_install/exps/

#make temp fs
cd ./busybox/_install/
find . | cpio -o --format=newc > ../../rootfs.img
cd ../../

#boot qemu
qemu-system-x86_64 -kernel /root/source/linux-3.2.10/arch/x86_64/boot/bzImage -initrd ./rootfs.img -append "console=ttyS0,ttyS1 root=/dev/ram rdinit=/sbin/init" -cpu kvm64 --nographic -gdb tcp::1234
