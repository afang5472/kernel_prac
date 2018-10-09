copy_from_user — Copy a block of data from user space.

prototype:  
  
```
unsigned long copy_from_user (	void *  	to,
 	const void __user *  	from,
 	unsigned long  	n);
```

```
Arguments:
to
Destination address, in kernel space.

from
Source address, in user space.

n
Number of bytes to copy.

```

