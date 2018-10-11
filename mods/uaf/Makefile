obj-m := heap.o
KERNELDR := /root/source/linux-3.2.10/ 
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDR) M=$(PWD) clean
