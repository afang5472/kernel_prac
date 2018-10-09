宏定义：#define MKDEV(major,minor) (((major) << MINORBITS) | (minor))

定义在<linux/cdev.h>中的宏    

形式：MKDEV(int major,int minor)    major为主设备号   minor为次设备号

成功执行返回dev\_t类型的设备编号，dev\_t类型是unsigned int 类型，32位，用于在驱动程序中定义设备编号，高12位为主设备号，低20位为次设备号.  
  
   内核中的每一个字符设备驱动程序都由一个 char\_device\_struct 结构体来描述，包含主设备号、起始次设备号、次设备号个数等信息。

内核使用 chrdevs 这个指针数组来管理所有的字符设备驱动程序，数组范围 0-255.  
  
that is to say:  
chrdevs[256].  
  
   内核使用 chrdevs 这个指针数组来管理所有的字符设备驱动程序，数组范围 0-255 ，看上去好像还是只支持 256 个字符设备驱动程序，其实并不然，每一个 char\_device\_struct 结构都含有一个 next 指针，它可以指向与其主设备号相同的其它字符设备驱动程序，它们之间主设备号相同，各自的次设备号范围相互不重叠。

