obj-m := target.o
KERNELDR := ~/linux_kernel/source/linux-4.4.158/ 
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDR) M=$(PWD) clean
