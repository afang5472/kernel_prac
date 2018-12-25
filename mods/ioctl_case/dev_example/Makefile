obj-m := ioctl.o
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNSRC_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNSRC_DIR) M=$(PWD) clean
