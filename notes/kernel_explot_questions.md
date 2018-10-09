iretq 之后，为什么只能执行一条syscall，返回的时候就会crash？。  

劫持执行流之后为什么自己跪了...

因为没有正确处理内核切换上下文。  

swapgs; iretq;  

