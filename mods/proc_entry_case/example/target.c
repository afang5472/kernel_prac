//stack_smashing.c
//This file intends for illustrating stack smashing technique.

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

int bug2_write(struct file *file,const char *buf,unsigned long len){
    char localbuf[8];
    memcpy(localbuf,buf,len);
    return len;
}

static int __init stack_smashing_init(void){
    printk(KERN_ALERT"stack smashing driver init!\n");
    create_proc_entry("bug2",0666,0)->write_proc = bug2_write;
    return 0;
}

static int __exit stack_smashing_exit(void){
    printk(KERN_ALERT"stack smashing driver exit!\n");
}

module_init(stack_smashing_init);
module_exit(stack_smashing_exit);
/*
makefile
obj-m := stack_smashing.o
KERNELDR := /mnt/hgfs/Qemu/x86/linux-2.6.32 
PWD := $(shell pwd)
modules:
        $(MAKE) -C $(KERNELDR) M=$(PWD) modules
modules_install:
        $(MAKE) -C $(KERNELDR) M=$(PWD) modules_install
clean:
        $(MAKE) -C $(KERNELDR) M=$(PWD) clean
*/

