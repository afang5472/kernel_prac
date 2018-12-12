obj-m := heap.o
KERNELDR := $(KERNSRC_DIR) 
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDR) M=$(PWD) clean
