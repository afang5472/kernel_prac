#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include<linux/slab.h>
#include<linux/string.h>

struct class *heap_class;
struct cdev cdev;
char *p;
int heap_major=201; //major device number.
struct param
{
    size_t len; //content length
    char* buf; //userspace buffer
    unsigned long idx;//ptr array index.
};
char *ptr[40]; //global ptrs
long heap_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct param par;
    struct param* p_arg;
    long p_stack;
    long* ptr;
    struct thread_info * info;
    copy_from_user(&par, arg, sizeof(struct param)); //copy struct param from user to kernel stack space.
    
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
static const struct file_operations heap_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = heap_ioctl,//linux 2.6.36内核之后unlocked_ioctl取代ioctl
};

static int heap_init(void)
{
	//device register
    //设备号
    dev_t devno = MKDEV(heap_major, 0);
    int result;
	//if heap_major is set , invoke register_chrdev_region to allocate device no.
    if (heap_major)
        result = register_chrdev_region(devno, 1, "heap");
    else {//dynamic main devno allocate procedure.
        result = alloc_chrdev_region(&devno, 0, 1, "heap");
        heap_major = MAJOR(devno);
    }
    //print dev number.
    printk("arw_major /dev/heap: %d", heap_major);
    if (result < 0)
        return result;
    heap_class = class_create(THIS_MODULE, "heap"); //class create? do not know this clear.
    device_create(heap_class, NULL, devno, NULL, "heap");
    cdev_init(&cdev, &heap_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, devno, 1);
    printk("heap dev init success\n");
    return 0;
}

static void heap_exit(void)
{
	//clean things created carefully..
    cdev_del(&cdev);
    device_destroy(heap_class, MKDEV(heap_major, 0));
    class_destroy(heap_class);
    unregister_chrdev_region(MKDEV(heap_major, 0), 1);
    printk("heap exit success\n");
}
MODULE_AUTHOR("exp_afang");
MODULE_LICENSE("GPL");
module_init(heap_init);
module_exit(heap_exit);

