obj-m := kmod_hellomod.o
KERNELDR := $(KERNSRC_DIR) 
PWD := $(shell pwd)
ccflags-y := -fno-stack-protector
all:
	$(MAKE) -C $(KERNELDR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDR) M=$(PWD) clean
