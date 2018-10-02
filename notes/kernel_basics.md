How to detect in runtime is KASLR enabled or disabled?  
--

Check your kernel command line.  
  
$ cat /proc/cmdline

```
root@debian-i386:~/mods# cat /proc/kallsyms  | grep sys_open
c104e31c W compat_sys_open_by_handle_at
c10cc788 T do_sys_open
c10cc855 T sys_open
c10cc878 T sys_openat
c10fe7c7 T sys_open_by_handle_at
c1109abb t proc_sys_open
```

second:  

```
c104e31c W compat_sys_open_by_handle_at
c10cc788 T do_sys_open
c10cc855 T sys_open
c10cc878 T sys_openat
c10fe7c7 T sys_open_by_handle_at
c1109abb t proc_sys_open
```

These kinds of attacks rely on knowing where symbols of interest live in the kernel's address space.  
 
Kernel module making command:  
--


```
obj-m := stk.o
LINUX_KERNEL := $(shell uname -r)
KERNELDR := /usr/src/linux-headers-$(LINUX_KERNEL) 
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KERNELDR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDR) M=$(PWD) clean
```
注意Tab缩进。  
  
First stackoverflow!
--

```
Our aim is to get a root shell.
For achieving our aim we should have two steps:

commit_creds(prepare_kernel_cred(0)) for elevating privilege in kernel mode.
system("/bin/sh") for getting shell in user mode
```


**The amount of space available for the kernel text to move around in is another problem. The code must be 2M aligned because of page table restrictions, and the space available is 2G. In a "perfect world", that would give 1024 slots for the location. In the real world, it turns out to be a fair amount less.**




>>e820 is shorthand to refer to the facility by which the BIOS of x86-based computer systems reports the memory map to the operating system or boot loader.
It is accessed via the int 15h call, by setting the AX register to value E820 in hexadecimal. It reports which memory address ranges are usable and which are reserved for use by the BIOS.
BIOS-e820 is often the first thing reported by a booting Linux kernel, and it can also be seen with the dmesg command.


