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

Supervisor Mode Access Prevention
--

Supervisor Mode Access Prevention (SMAP) is a feature of some CPU implementations such as the Intel Broadwell microarchitecture that allows supervisor mode programs to optionally set user-space memory mappings so that access to those mappings from supervisor mode will cause a trap.  
  

SMEP -- Supervisor Mode Execution Prevention  
SMAP -- Supervisor Mode Access Prevention  
  
How to acquire a root shell without kernel panic?  
--  

if we can:  

- commit_creds(prepare_kernel_cred(0)) for elevating privilege in kernel mode.
- system("/bin/sh") for getting shell in user mode


So we can control return address to executecommit_creds(prepare_kernel_cred(0)) in bug2_write function kernel mode.
But stack is trashed, so we can’t return normally. We could ﬁx up the stack, but that’s boring.
Instead, let’s jump directly to user mode.

swapgs
--

SWAPGS把当前GS基寄存器值和在MSR地址C0000102H(IA32_KERNEL_GS_BASE)所包 含的值进行交换。SWAPGS指令是一个为系统软件设计的特权指令。(….)内核可 以使用GS前缀在正常的内存引用去访问[per-cpu]内核数据结构。”  
  

Linux内核为每个CPU在启动时分配一个固定大小的结构体来存放关键数据。之后 为每个CPU加载IA32_KERNEL_GS_BASE到相应的结构地址上，因此，通常的情况， 比如系统调用的处理程序是：

- swapgs（现在是GS指向内核空间）

- 通过内存指令和gs前缀访问per-cpu内核数据结构

- swapgs（撤销之前的swapgs，GS指向用户空间）

- 返回用户空间

内核代码天生必须保证随时能访问到带gs前缀的percpu数据，内核执行一堆 swapgs指令时条目来自用户空间并不是偶然。(所以gs base指向内核内存）  




