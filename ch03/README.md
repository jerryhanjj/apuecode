## 第三章 文件I/O

### 代码
- [第三章代码 Github](https://github.com/jerryhanjj/apuecode/tree/master/ch03)

### 3.1 引言

常用不带缓冲(unbuffered)的I/O
- open
- read
- write
- lseek
- close

**不带缓冲：** 每个`read`和`write`都调用内核中的一个系统调用，是`POSIX.1`和`Signal UNIX Specification`的组成部分，不是`ISO C`组成部分。

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

|   常量   |                         描述                          | 值  | 选项 |
| -------- | ----------------------------------------------------- | --- | ---- |
| O_RDONLY |                       只读打开                        |  0  | 必选 |
| O_WRONLY |                       只写打开                        |  1  | 必选 |
|  O_RDWR  |                        读、写                         |  2  | 必选 |
|  O_EXEC  |                        只打开                         |     | 必选 |
| O_SEARCH |      在打开目录时验证搜索权限，很多系统还不支持       |     | 必选 |
| O_APPEND |                     在文件尾追加                      |     | 可选 |
| O_CREAT  |    若文件不存在则创建新文件，需指定文件的访问权限     |     | 可选 |
| O_TRUNC  | 如果文件存在，且只写或读写成功打开，则将其长度截断为0 |     | 可选 |
|  O_SYNC  |   每次write等待物理I/O操作完成，包括文件属性更新I/O   |     | 可选 |

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
```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```

### 3.8 函数 write
```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```
返回值通常与参数`count`相同，否则表示出错。对于普通文件，写操作从文件当前偏移量开始，默认`offset`是**0**，如果打开文件是指定`O_APPEND`选项，则偏移量在文件结尾处。写成功之后，偏移量增加实际写的字节数。

### 3.9 I/O的效率
- mycat.c

```c
$ ./mycat
ls
ls
haha
haha

$ ./mycat > data
ls
hello
$ cat data
ls
hello

$ ./mycat < data > data.copy
$ cat data.copy
ls
hello
```

### 3.10 文件共享
**整节反复阅读**

### 3.11 原子操作
- read
- write

在多进程写同一个文件时，如果没有保证原子性（锁），则会发生意料之外的结果。
- pread
- pwrite

`pread`是`read`的原子性操作，相当于无法中断的`lseek`和`read`操作，同时不更新文件偏移量。`pwrite`类似。

### 3.12 函数dup和dup2
复制一个现有的文件描述符，另一种方式是使用`fcntl`函数。
dup的作用是将函数返回的文件标志指向和自己一样的文件表项，而open的作用是使得函数返回的描述符指向另外一个文件表项，但是两者都会指向同一个V节点，
```C
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```
dup2(fd, fd2)**不完全等效于**
```C
close(fd2)
fcntl(fd, F_DUPFD, fd2)
```
dup2是原子操作，而后者不是，在close和fcntl之间程序可能挂起，执行其他有关文件的操作。

### 3.13 函数sync、fsync、fdatasync
```c
#include <unistd.h>

int fsync(int fd);
int fdatasync(int fd);
void sync(void);
```
**区别：**
- sync 不等待写磁盘操作结束
- fdatasync 只影响文件数据部分，fsync 还包括更新文件属性

### 3.14 函数fcntl
操作文件描述符
**Linux 中 通过 fnctl 设置 O_SYNC 似乎不起作用，但是 open 时指定 O_SYNC 参数有用。**
```c
#include <unistd.h>
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */ );
```

1. 复制一个已有的描述符（ cmd=F_DUPFD 或 F_DUPFD_CLOEXEC ）
2. 获取 / 设置文件描述符标志（ cmd=F_GETFD 或 F_SETFD ）
3. 获取 / 设置文件状态标注（ cmd=F_GETFL 或 F_SETFL ）
4. 获取 / 设置异步 I/O 所有权（ cmd=F_GETOWN 或 F_SETOWN ）
5. 获取 / 设置记录锁（ cmd=F_GETLK 、 F_SETLK 或 F_SETLKW ）

|      CMD参数       |                                                  描述                                                  |
| --------------- | ------------------------------------------------------------------------------------------------------ |
|     F_DUPFD     | 复制文件描述符。新文件描述符作为函数返回值返回，与 fd 共享同一文件表项，清除 FD_CLOEXEC 文件描述符标志 |
| F_DUPFD_CLOEXEC |                                  复制文件描述符，设置 FD_CLOEXEC 标志                                  |
|     F_GETFD     |                                   返回文件描述符标志，如 FD_CLOEXEC                                    |
|     F_SETFD     |                                  设置 fd 的描述符标志，如 FD_CLOEXEC                                   |
|     F_GETFL     |                            获取 fd 的文件状态标志，如 O_RDONLY,O_WRONLY...                             |
|     F_SETFL     |            设置文件状态标志，如 O_APPEND,O_NONBLOCK,O_SYNC,O_DSYNC,O_RSYNC,O_FSYNC,O_ASYNC             |


**O_ACCMODE：**
这个宏作为一个掩码以与文件状态标识值做AND位运算，产生一个表示文件访问模式的值。这模式将是`O_RDONLY`, `O_WRONLY`, 或 `O_RDWR`（在GNU系统中，也可能是零，不包括 O_EXEC 位）。
- O_ACCMODE：**0011**
- O_RDONLY：**00**
- O_WRONLY：**01**
- O_RDWR：**10**

通过“与”运算**取出flag中的低两位**
```
（val & O_ACCMODE）
```

**功能测试宏：**
[UNIX功能测试宏](https://blog.csdn.net/jerryhanjj/article/details/104417753 "UNIX功能测试宏")

- fileflag.c

```shell
$ ./fileflag 0
read write, append

$ ./fileflag 1
read write, append

$ ./fileflag 0 < /dev/tty	# 标准输入0重定向为文件tty的描述符
read only

$ ./fileflag 1 > temp.foo	# 标准输出1 重定向为文件temp.foo，即把printf输出打印在文件temp.foo中

$ cat temp.foo
write only

$ ./fileflag 0 1>temp.foo 	# 执行命令 ./fileflag 0 并把输出定向到文件temp.foo中

$ cat temp.foo
read write, append

$ ./fileflag 5
fcntl error for fd 5: Bad file descriptor

$ ./fileflag 5 2>>temp.foo	# 命令 ./fileflag 5 的标准输出 >> 追加到文件中

$ cat temp.foo
read write, append
fcntl error for fd 5: Bad file descriptor

$ ./fileflag 5 5<>temp.foo	# 以可读写的方式打开文件temp.foo，并把文件描述符作为5号描述符执行命令 ./fileflag 5
read write
```
**生成测试用大文件：**
[Linux 测试用大文件生成](https://blog.csdn.net/jerryhanjj/article/details/104419485 "Linux 测试用大文件生成")

**Linux 中 通过 fnctl 设置 O_SYNC 似乎不起作用，但是 open 时指定 O_SYNC 参数有用。**
- setfcntl
- 修改 mycat.c，使用 set_fl 或者 open

```shell
# 生成100M的测试文件
$ dd if=/dev/zero of=test.file bs=1M count=100

# 使用 set_fl 查看所用时间
$ time ./mycat < test.file > mycat.file
real	0m1.310s
user	0m0.069s
sys	0m1.240s

# 使用 open ，查看所用时间
$ time ./mycat < test.file
real	0m54.776s
user	0m0.006s
sys	0m7.068s

# 因为是在虚拟机里面跑的所以有点慢 😳
```

### 3.15 函数 ioctl
```c
#include <sys/ioctl.h>

int ioctl(int fd, unsigned long request, ...);
```

### 3.16 /dev/fd
描述符文件存放目录
```shell
fd = open("/dev/fd/0", mode);
# 等效于
fd = dup(0);	# 描述符0 和 fd 共享同一文件表项
```
**一般不能（不管mode是什么方式）也不建议对这种方式打开的`fd`进行操作。
在 `Linux` 操作系统上， `/dev/fd/0`是个例外，系统标准文件描述符（0，1，2...)映射成指向底层物理文件的符号链接，因此在它上面调用 `creat` 会导致底层文件被截断。**

### 习题

**3.1 所有磁盘I/O都要经过内核块缓冲区。“不带缓冲I/O”指的是在用户进程中例如`read`和`write`这两个函数不会自动缓冲，每执行一次就要进行一次系统调用。**

**3.2 实现与dup2相同的功能，且不使用fnctl**

- e32.c

```shell
$ ./e32
Usage: <fd>

$ ./e32 2048
error: fd2 beyond OPEN_MAX
usage: fd2 < 1024

$ ./e32 6
close fd: 32573
close fd: 5
close fd: 4
close fd: 3
my dup test...
```

**3.3 `open`在内核中创建新的文件表项，而`dup`复制的`fd`则共享同一个文件表项，`F_SETFD`只作用于当前`fd`，而`F_SETFL`作用于指向同一个文件表项的所有`fd`**

```shell
$ sudo ./e33
fd 0 : FD_CLOEXEC=0 mode=32770
fd 1 : FD_CLOEXEC=0 mode=32770
fd 2 : FD_CLOEXEC=0 mode=32768
--------修改fd[0] FD-------
fd 0 : FD_CLOEXEC=1 mode=32770
fd 1 : FD_CLOEXEC=0 mode=32770
fd 2 : FD_CLOEXEC=0 mode=32768
--------修改fd[2] FD-------
fd 0 : FD_CLOEXEC=1 mode=32770
fd 1 : FD_CLOEXEC=0 mode=32770
fd 2 : FD_CLOEXEC=1 mode=32768
--------修改fd[0] FL-------
fd 0 : FD_CLOEXEC=1 mode=33794
fd 1 : FD_CLOEXEC=0 mode=33794
fd 2 : FD_CLOEXEC=1 mode=32768
--------修改fd[2] FL-------
fd 0 : FD_CLOEXEC=1 mode=33794
fd 1 : FD_CLOEXEC=0 mode=33794
fd 2 : FD_CLOEXEC=1 mode=33792
```

**3.4 如果fd是1，执行三次后，描述符0，1，2都指向1，如果fd是3，则0，1，2，3都指向1，需要关闭3（为什么？？？）**

**3.5 在Bourne shell、Bourne-again、Korn shell中，digit1>&digit2 表示要将描述符 digit1 重定向至描述符 digit2 的同一文件。请说明下面两条命令的区别。**

- e35.c
```shell
$ ./e35 > outfile 2>&1
$ cat outfile
out
err
$ ./e35 2>&1 > outfile
err
$ cat outfile
out
```

**`2>&1`表示将标准错误重定向标准输出，在控制台输出错误信息，相当于`dup(STDERR_FILENO, STDOUT_FILENO)`，同时`dup`共享同一个文件表项，`write` 时是追加的方式，所以命令一两行都打印在`outfile`中。命令二先将`stderr`重定向`stdout`，错误信息由控制台输出，然后`stdout`定向到`outfile`，打印`out`。**

**3.6 以O_APPEND方式打开文件，仍然可用lseek和read函数读取文件任意一个位置的内容，但是write只能在文件尾，在write之前文件偏移量会自动设置为文件尾。**

