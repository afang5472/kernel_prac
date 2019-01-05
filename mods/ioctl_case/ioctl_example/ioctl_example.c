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

//this test case illstrates ioctl usage in kernel module.

struct class *example_class;
struct cdev cdev;
char *p;

int MAJOR_DEV_NO = 200;

struct param
{
    size_t len;
    char* buf;
    char* addr;
};
char buf[16] = {0};
long example_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct param par;
    struct param* p_arg;
    long p_stack;
    struct thread_info * info;
    int retval = 0;
    copy_from_user(&par, (void*)arg, sizeof(struct param)); //arg is a param pointer.
    
    switch (cmd) {
        case 8:
            printk("current: 0x%p, size: %zd, buf:%p\n", current, par.len, par.buf);
            copy_from_user(buf, par.buf, par.len);
            break;
        case 7:
            printk("buf(%p), content: %s\n", buf, buf);
            break;
        case 5:
            p_arg = (struct param*)arg;
            p_stack = (long)&retval;
            p_stack = p_stack&0xFFFFFFFFFFFFC000;
            info = (struct thread_info * )p_stack;
            
            printk("addr_limit's addr: 0x%p\n", &info->addr_limit);
            memset(&info->addr_limit, 0xff, 0x8);
            put_user(info, &p_arg->addr);
            break;
        case 999:
            p = kmalloc(8, GFP_KERNEL);
            printk("kmalloc(8) : %p\n", p);
            break;
        case 888:
            kfree(p);
            printk("kfree : %p\n", p);
            break;
        default:
            retval = -1;
            break;
    }
    return retval;
}
static const struct file_operations example_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = example_ioctl
};

static int example_init(void)
{
    dev_t devno = MKDEV(MAJOR_DEV_NO, 0); //MKDEV --> major_dev_no, secondary_dev_no.
    int result;
    if (MAJOR_DEV_NO){ //if MAJOR_DEV_NO is set, then register it.
        result = register_chrdev_region(devno, 1, "example");
	}
    else {
		//else alloc it as a new dev no.
        result = alloc_chrdev_region(&devno, 0, 1, "example");
        MAJOR_DEV_NO = MAJOR(devno);
    }
    printk("ioctl example created at /dev/example: %d(MAJOR Device Number)\n", MAJOR_DEV_NO);
    if (result < 0)
		//error 
        return result;
    example_class = class_create(THIS_MODULE, "example");
    device_create(example_class, NULL, devno, NULL, "example");
    cdev_init(&cdev, &example_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, devno, 1);
    printk("example module init finished\n");
    return 0;
}
static void example_exit(void)
{
    cdev_del(&cdev);
    device_destroy(example_class, MKDEV(MAJOR_DEV_NO, 0)); //class->device.
    class_destroy(example_class);
    unregister_chrdev_region(MKDEV(MAJOR_DEV_NO, 0), 1);
    printk("example module exit success\n");
}
MODULE_LICENSE("GPL");
module_init(example_init);
module_exit(example_exit);
