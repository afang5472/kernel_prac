#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>

struct class *arw_class;
struct cdev cdev;
char *p;
int arw_major=200;
struct param
{
    size_t len;
    char* buf;
    char* addr;
};
char buf[16] = {0};
long arw_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct param par;
    struct param* p_arg;
    long p_stack;
    long* ptr;
    struct thread_info * info;
    copy_from_user(&par, arg, sizeof(struct param));
    
    int retval = 0;
    switch (cmd) {
        case 8:
            printk("current: %p, size: %d, buf:%p\n", current, par.len, par.buf);
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
            // 返回 thread_info 的地址， 模拟信息泄露
            put_user(info, &p_arg->addr);
            break;
        case 999:
            p = kmalloc(8, GFP_KERNEL);
            printk("kmalloc(8) : %p\n", p);
            break;
        case 888://数据清零
            kfree(p);
            printk("kfree : %p\n", p);
            break;
        default:
            retval = -1;
            break;
    }
    return retval;
}
static const struct file_operations arw_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = arw_ioctl,//linux 2.6.36内核之后unlocked_ioctl取代ioctl
};

static int arw_init(void)
{
    //设备号
    dev_t devno = MKDEV(arw_major, 0);
    int result;
    if (arw_major)//静态分配设备号
        result = register_chrdev_region(devno, 1, "arw");
    else {//动态分配设备号
        result = alloc_chrdev_region(&devno, 0, 1, "arw");
        arw_major = MAJOR(devno);
    }
    // 打印设备号
    printk("arw_major /dev/arw: %d\n", arw_major);
    if (result < 0)
        return result;
    arw_class = class_create(THIS_MODULE, "arw");
    device_create(arw_class, NULL, devno, NULL, "arw");
    cdev_init(&cdev, &arw_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, devno, 1);
    printk("arw init success\n");
    return 0;
}
static void arw_exit(void)
{
	//clean things created carefully..
    cdev_del(&cdev);
    device_destroy(arw_class, MKDEV(arw_major, 0));
    class_destroy(arw_class);
    unregister_chrdev_region(MKDEV(arw_major, 0), 1);
    printk("arw exit success\n");
}
MODULE_AUTHOR("exp_ttt");
MODULE_LICENSE("GPL");
module_init(arw_init);
module_exit(arw_exit);

