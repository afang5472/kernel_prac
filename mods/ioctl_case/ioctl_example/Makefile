obj-m := ioctl_example.o
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNSRC_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNSRC_DIR) M=$(PWD) clean
