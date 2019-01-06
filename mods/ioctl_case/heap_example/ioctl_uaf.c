#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>

//:This module illustrates heap UAF in kernel.

struct class *uaf_class;
struct cdev cdev;
char *p;

int MAJOR_DEV_NO = 201;

struct param
{
    size_t len;
    char* buf;
	unsigned long idx; //allocated chunks stored in ptr list's index.
};

char *ptr[40];

long uaf_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct param* p_arg;
    int retval = 0;
	p_arg = (struct param*)arg; //arg comes from userspace...
    
    switch (cmd) {
        case 'a':
			copy_to_user(p_arg->buf, ptr[p_arg->idx], p_arg->len);
			printk("copy_to_user from: 0x%p\n", (size_t*)ptr[p_arg->idx]);
            break;
        case 'b':
			copy_from_user(ptr[p_arg->idx], p_arg->buf, p_arg->len); //arbitrary write through base pointer...
            break;
        case 'c': //release...
			kfree(ptr[p_arg->idx]); //use after free...
			printk("freed ptr at: 0x%p\n", ptr[p_arg->idx]);
            break;
        case 'd'://allocate... 
			ptr[p_arg->idx] = kmalloc(p_arg->len, GFP_KERNEL);
            printk("kmalloc at: 0x%p with size: %2x\n", ptr[p_arg->idx], p_arg->len);
            break;
        default:
            retval = -1;
            break;
    }
    return retval;
}
static const struct file_operations uaf_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = uaf_ioctl
};

static int uaf_init(void)
{
    dev_t devno = MKDEV(MAJOR_DEV_NO, 0); //MKDEV --> major_dev_no, secondary_dev_no.
    int result;
    if (MAJOR_DEV_NO){ //if MAJOR_DEV_NO is set, then register it.
        result = register_chrdev_region(devno, 1, "uaf");
	}
    else {
		//else alloc it as a new dev no.
        result = alloc_chrdev_region(&devno, 0, 1, "uaf");
        MAJOR_DEV_NO = MAJOR(devno);
    }
    printk("ioctl_uaf mod created at /dev/uaf: %d(MAJOR Device Number)\n", MAJOR_DEV_NO);
    if (result < 0)
		//error 
        return result;
    uaf_class = class_create(THIS_MODULE, "uaf");
    device_create(uaf_class, NULL, devno, NULL, "uaf");
    cdev_init(&cdev, &uaf_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, devno, 1);
    printk("uaf module init finished\n");
    return 0;
}
static void uaf_exit(void)
{
    cdev_del(&cdev);
    device_destroy(uaf_class, MKDEV(MAJOR_DEV_NO, 0)); //class->device.
    class_destroy(uaf_class);
    unregister_chrdev_region(MKDEV(MAJOR_DEV_NO, 0), 1);
    printk("uaf module exit success\n");
}
MODULE_LICENSE("GPL");
module_init(uaf_init);
module_exit(uaf_exit);
