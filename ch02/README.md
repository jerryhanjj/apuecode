## 第二章 UNIX标准及实现

### 习题
**2.1 使用头文件保护防止多次定义**
```C++
/***** headfile.h *****/
#ifndef _HEADFILE_H
#define _HEADFILE_H
...
#endif
```

**2.2 基本系统数据类型**  
[查找系统数据类型，以 clock_t 为例](https://blog.csdn.net/jerryhanjj/article/details/104382421 "查找系统数据类型，以 clock_t 为例")

**2.3 如果`sysconf`返回的值表示`OPEN_MAX`是未确定的，或者将`LONG_MAX`作为`OPEN_MAX`值来报告时，使用`getrlimit`得到每个进程的最大打开文件描述符数。**
`OPEN_MAX`宏定义现在已经被取消，因为其值在运行时可能会改变，最好动态获取，现在变成了资源限制`RLIMIT_NOFILE`的一部分，关于`getrlimit`查看`man`手册获得更多信息。
```c
#include "apue.h"
#include <limits.h>
#include <sys/resource.h>

#define OPEN_MAX_GUESS 256

long open_max(void)
{
	long openmax;
	struct rlimit rl;
	if((openmax = sysconf(_SC_OPEN_MAX)) < 0 || openmax == LONG_MAX)
	{
		if(getrlimit(RLIMIT_NOFILE, &rl) < 0)
		{
			err_sys("can't get file limit");
		}
		if(rl.rlim_max == RLIM_INFINITY)
		{
			openmax = OPEN_MAX_GUESS;
		}else
		{
			openmax = rl.rlim_max;
		}
	}
	return(openmax);
}
```
