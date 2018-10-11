在Linux2.6内核中一个字符设备用cdev结构来描述，其定义如下：

```
struct cdev {
        struct kobject kobj;
        struct module *owner;   //所属模块
        const struct file_operations*ops;   
               //文件操作结构，在写驱动时，其结构体内的大部分函数要被实现
        struct list_head list;
        dev_tdev;          //设备号，int 类型，高12位为主设备号，低20位为次设备号
        unsigned int count;
};
```

