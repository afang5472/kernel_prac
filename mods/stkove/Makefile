obj-m := target.o
KERNELDR := /root/source/linux-3.2.10/ 
PWD := $(shell pwd)
ccflags-y := -fno-stack-protector
all:
	$(MAKE) -C $(KERNELDR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDR) M=$(PWD) clean
