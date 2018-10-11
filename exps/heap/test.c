#include "template.h"

struct param{

	size_t len;
	char *buf;
	size_t idx;
};

int main(){

	printf("heap exploit module!");
	struct param p;
	p.len = 0x2e0;
	p.buf = malloc(p.len);
	for(int i = 0; i < 10; i++){
		p.idx = i;
		ioctl();
	}
	return 0;
}
