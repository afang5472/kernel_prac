#!/bin/bash 

rm ~/linux_kernel/busybox/_install/*.ko
rm ~/linux_kernel/busybox/rootfs.img
cp ~/linux_kernel/mod/target.ko ~/linux_kernel/busybox/_install/
cd ~/linux_kernel/busybox/_install/

find . | cpio -o --format=newc > ~/linux_kernel/busybox/rootfs.img
cd ~/linux_kernel/

qemu-system-x86_64 -kernel ./source/linux-4.4.158/arch/x86_64/boot/bzImage -initrd ./busybox/rootfs.img -append "console=ttyS0 root=/dev/ram rdinit=/sbin/init" -cpu kvm64,+smep,+smap --nographic -gdb tcp::1234


