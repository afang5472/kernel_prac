#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

//author : afang
//comment: what a nice day, isn't it?!

int main(){

	//interact with kernel module to test if module is effective..
	char *file = "/proc/hellomod";
	char *buf = "I'm inside kernel.\n";
	int fd = open(file, O_RDWR);
	write(fd, buf, strlen(buf));
	char *buf2 = (char*)malloc(20);
	read(fd, buf2, 20);
	printf("content:\n");
	printf("%s", buf2);
	return 0;
}

