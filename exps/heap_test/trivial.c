#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>

//author : afang
//comment: what a nice day, isn't it?!

int main(int argc, char* argv[]){

	char *dev = "/dev/uaf";
	char *buf = "I'm inside kernel.\n";
	int fd = open(dev, O_RDWR);

	struct temp{
		size_t length;
		char *buf;
		size_t idx;
	}a1;
	a1.length = 0x2e0;
	a1.idx = 20;
	struct temp *test = &a1;

	for(int i = 0; i < 1000; i++){
		ioctl(fd, 'd', test);
	}
	return 0;
}
