## 第三章 文件I/O

### 代码
- [github](https://github.com/jerryhanjj/apuecode/tree/master/ch03)

### 3.1 引言

常用不带缓冲(unbuffered)的I/O
- open
- read
- write
- lseek
- close

不带缓冲：每个`read`和`write`都调用内核中的一个系统调用，是`POSIX.1`和`Signal UNIX Specification`的组成部分，不是`ISO C`组成部分。

### 3.2 文件描述符
文件描述符
- 范围：0~OPEN_MAX-1
现在范围常常受到系统、soft limit和hard limit的约束
- 符号常量：<unistd.h>

|   描述   |  符号  | 值  |   符号常量    |
| -------- | ------ | --- | ------------- |
| 标准输入 | stdin  |  0  | STDIN_FIFLNO  |
| 标准输出 | stdout |  1  | STDOUT_FILENO |
| 标准错误 | stderr |  2  | STDERR_FILENO |

### 3.3 函数 open 和 openat

- open
- openat
`openat`可以让线程使用相对路径名打开不同的工作目录

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);

int openat(int dirfd, const char *pathname, int flags);
int openat(int dirfd, const char *pathname, int flags, mode_t mode);
```

文件打开方式参数  
在必选参数中**选择一个且只能选择一**个，可与可选项“或”运算构成参数，例如：
```c
open(pathname, O_RDWR|O_APPEND, ...)
```

|   常量   |                      描述                      | 值  | 选项 |
| -------- | ---------------------------------------------- | --- | ---- |
| O_RDONLY |                    只读打开                    |  0  | 必选 |
| O_WRONLY |                    只写打开                    |  1  | 必选 |
|  O_RDWR  |                     读、写                     |  2  | 必选 |
|  O_EXEC  |                     只打开                     |     | 必选 |
| O_SEARCH |   在打开目录时验证搜索权限，很多系统还不支持   |     | 必选 |
| O_APPEND |                  在文件尾追加                  |     | 可选 |
| O_CREAT  | 若文件不存在则创建新文件，需指定文件的访问权限 |     | 可选 |

更多参数查看`man`手册获得详细说明，头文件`<fcntl.h>`

### 3.4 函数 creat
新版本open函数之前使用，现在已被摒弃。
```c
int creat(const char *pathname, mode_t mode);
```
函数等效于：
```c
open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
```

### 3.5 函数close
关闭打开的文件，释放进程加在文件上的所有记录锁，进程终止时，内核也会自动关闭它所打开的文件。

### 3.6 函数lseek
调用lseek显式为一个打开文件设置偏移量
```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```
lseek根据参数whence的值来设置文件的偏移量

|  whence  |               描述               |
| -------- | -------------------------------- |
| SEEK_SET |     距文件开始处offset个字节     |
| SEEK_CUR | 距当前位置offset，offset可正可负 |
| SEEK_END | 文件长度加offset，offset可正可负 |

如果fd指向管道、FIFO或网络套接字，lseek返回-1，将errno设置为ESPIPE
- canseek.c

```shell
$ ./canseek < /etc/passwd
seek ok
$ ./canseek < /etc/passwd | ./canseek   
can't seek
```

- seekhole.c

```shell
$ ./seekhole  

$ ls -l file.hole 
-rw-r--r-- 1 tf tf 16394 2月  19 19:56 file.hole
# 显示文件大小为16394

$ od -c file.hole 
0000000   a   b   c   d   e   f   g   h   i   j  \0  \0  \0  \0  \0  \0
0000020  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0
*
0040000   A   B   C   D   E   F   G   H   I   J
0040012
```
每行开始的八进制数表示字节的偏移量，`0040012`转换成十进制便是**16394**

### 3.7 函数 read



























