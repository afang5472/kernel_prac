#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

//author : afang
//comment: what a nice day, isn't it?!
//linux kernel module template 

int len, temp;
char *msg;

int read_proc(struct file *filep, char *buf, size_t count, loff_t *offp){
	char local_buf[20];
	strcpy(local_buf, buf);
	return copy_to_user(buf, local_buf, 20);
}

int write_proc(struct file *filep, const char *buf, size_t count, loff_t *offp){
	copy_from_user(msg,buf,count);
	len = count;
	temp = len;
	return count;
}

struct file_operations proc_fops = {
read: read_proc,
write: write_proc 
};

void create_new_proc_entry(void){

//	msg = kmalloc(10*sizeof(char), GFP_KERNEL); //kmalloc size 10 for usage.
//	printk("space allocated at : %p\n", msg);
	proc_create("stkovemod", 0666, NULL, &proc_fops);
}


int proc_init(void){

	create_new_proc_entry();
	return 0;
}

void proc_cleanup(void){
	remove_proc_entry("hellomod", NULL);
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

