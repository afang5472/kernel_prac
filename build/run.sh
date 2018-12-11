#!/bin/bash 

#rm ~/kernel_env/busybox-1.29.3/_install/*.ko

source ./env.sh

# delete former rootfs image>
rm $BUSYBOX_DIR"rootfs.img"

# adding necessary files
cd $BUSYBOX_INSTALL
# move drivers here>
mkdir -p mods_to_insert
echo "mods put here in exercises" > mods_to_insert/README

#copy target mods here
# todo:

# repack rootfs image>
find . | cpio -o --format=newc > ../rootfs.img 

qemu-system-x86_64 -kernel ./source/linux-4.4.158/arch/x86_64/boot/bzImage -initrd ./busybox/rootfs.img -append "console=ttyS0 root=/dev/ram rdinit=/sbin/init" -cpu kvm64,+smep,+smap --nographic -gdb tcp::1234
