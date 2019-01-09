#!/bin/bash 

#rm ~/kernel_env/busybox-1.29.3/_install/*.ko

source ./env.sh

# delete former rootfs image>
rm $BUSYBOX_DIR"temp_rootfs.img"

# adding necessary files
cd $BUSYBOX_INSTALL
# move drivers here>
#mkdir -p mods_to_insert
#echo "mods put here in exercises" > mods_to_insert/README

#mkdir -p exploits
#echo "exploits put here" > exploits/README

#copy target mods here
# todo:
# move the whole directory .
#cp -r $KERNPRAC_MOD .

#copy exp here 
# todo:
#cp -r $KERNPRAC_EXP .

cp -r $KERNPRAC_TESTS .

# repack rootfs image>
find . | cpio -o --format=newc > ../temp_rootfs.img 

#start..!
echo "Qemu Inits..."
/home/afang/QEMU/qemu/build/x86_64-softmmu/qemu-system-x86_64 -kernel $KERNSRC_DIR"/arch/x86_64/boot/bzImage" -initrd $BUSYBOX_DIR"temp_rootfs.img" -append "console=ttyS0 root=/dev/ram rdinit=/sbin/init" -cpu kvm64 -monitor /dev/null -nographic 
