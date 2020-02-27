## 第五章 标准I/O库

### 5.2 流和 FILE 对象
- 普通 I/O，文件描述符与文件关联
- 标准 I/O，流（stream）与文件关联

流的定向决定是**单字节流**还是 **多字节流(宽)**。
两个函数可以改变流的定向 `fwide` 和 `freopen`。

- fwide

```c
#include <wchar.h>
int fwide(FILE *stream, int mode);
```
**mode参数**:
- 参数为负：fwide 将**试图**指定流是**字节定向**
- 参数为正：fwide 将**试图**指定流是**宽定向**的
- 参数为 0：fwide 不设置流的定向，返回标识该流定向的值

**返回值：**
- 宽定向：“+”
- 字节定向： “-”
- 未定向： 0

**`fiwde` 不改变已定向流的定向，且无出错返回。在调用前清除 `errno`，返回值时检查 `errno` 的值，通过 `errno` 的值来判断成功还是失败。**。

**FILE 对象**

`fopen` 打开一个流时，返回一个 `FILE` 对象的指针，`FILE` 对象是一个包含了流的所有信息的**结构**，包括文件描述符、缓冲区指针、缓冲区长度、缓冲区字符数及出错标志等。


### 5.3 标准输入、标准输出和标准错误
流与文件的对应方式，与文件描述符与文件的对应方式相似。
头文件：`<stdio.h>`

|  流  |    文件I/O    | 标准I/O |
| ---- | ------------- | ------- |
| 输入 | STDIN_FILENO  |  stdin  |
| 输出 | STDOUT_FILENO | stdout  |
| 错误 | STDERR_FILENO | stderr  |

### 5.4 缓冲

**目的：**  减少使用 `read` 和 `write` 调用的次数，自动对每个 I/O 流进行缓冲管理。

#### 全缓冲

**标准 I/O** 填满缓冲区后才进行**实际的 I/O 操作**。磁盘上的文件就是这样。流第一次进行I/O操作时使用 `malloc` 获得缓冲区。

冲洗（flush）：缓冲区的写操作。缓冲区可由程序自动冲洗，也可调用 `fflush` 手动冲洗一个流。

**flush两种含义：**
1. 将缓冲区内容写到磁盘
2. 清除缓冲区中数据

#### 行缓冲
在输入和输出中遇到换行符，才执行I/O操作，比如 `fputc` 一次输出一个字符，但只有在写了一行之后才进行实际I/O操作。终端通常使用行缓冲

**限制：**
1. 每一行缓冲区长度固定，缓冲区满，**没有换行符**也进行I/O操作
2. 从不带缓冲的流，或者行缓冲的流获取数据，**冲洗所有行缓冲输出流**

#### 不带缓冲
标准I/O库不对字符进行缓冲存储。例如，用 `fputs` 写15个字符到不带缓冲流中，字符立即输出。可能使用 `write` 直接写入相关文件中。
`stderr` 通常不带缓冲。

**通常情况：**
- `stderr` 是不带缓冲的
- 指向终端设备的流，是行缓冲，否则是全缓冲

#### 更改缓冲类型
```c
#include <stdio.h>
void setbuf(FILE *stream, char *buf);
int setvbuf(FILE *stream, char *buf, int mode , size_t size);
```

- setbuf

关闭或打开缓冲机制。带缓冲（全缓冲，如果终端设备，可以为行缓冲）时，`buf` 指向长度为 `BUFSIZ` (定义在 `<stdio.h>` ) 的缓冲区；关闭缓冲，`buf` 设置为 `NULL`。

- setvbuf

利用 `mode` 参数，精确说明需要使用的缓冲类型。

- 二者区别

<table>
   <tr>
      <td>函数</td>
      <td>mode</td>
      <td>buf</td>
      <td>缓冲区及长度</td>
      <td>缓冲类型</td>
   </tr>
   <tr>
      <td rowspan="2">setbuf</td>
      <td rowspan="2"></td>
      <td>非空</td>
      <td>长度为 BUFSIZ 的用户缓冲区 buf</td>
      <td>全缓冲或行缓冲</td>
   </tr>
   <tr>
      <td>NULL</td>
      <td>无缓冲区</td>
      <td>不带缓冲</td>
   </tr>
   <tr>
      <td rowspan="5">setvbuf</td>
      <td rowspan="2">_IOFBF</td>
      <td>非空</td>
      <td>长度为 size 的用户缓冲区 buf</td>
      <td rowspan="2">全缓冲</td>
   </tr>
   <tr>
      <td>NULL</td>
      <td>合适长度的系统缓冲区 buf</td>
   </tr>
   <tr>
      <td rowspan="2">_IOLBF</td>
      <td>非空</td>
      <td>长度为 size 的用户缓冲区 buf</td>
      <td rowspan="2">行缓冲</td>
   </tr>
   <tr>
      <td>NULL</td>
      <td>合适长度的系统缓冲区 buf</td>
   </tr>
   <tr>
      <td>_IONBF</td>
      <td>（忽略）</td>
      <td>无缓冲区</td>
      <td>不带缓冲</td>
   </tr>
</table>

**自动分配的 `size` 长度的值通常是常量 `BUFSIZ` 指定的值。**

**注意**：
- 如果在一个函数内分配一个自动变量类的标准I/O缓冲区，则从函数返回之前，必须关闭该流。
- 缓冲区的一部分还用来存放**实现**自己的管理操作信息，所以存放在缓冲区的实际字节数少于 `size`。
- 一般应由系统选择缓冲区的长度，并自动分配缓冲区。这种情况下关闭流，标准I/O库自动释放缓冲区。

#### 强制冲洗一个流
- `fflush`
```c
#include <stdio.h>
int fflush(FILE *stream);
```

**返回值**
- 成功：0
- 失败：`EOF` 并 设置 `errno` 值

**此函数使流所有未写的数据传送至内核，作为例外，如果 `fp` 是 `NULL` ，则将导致所有的输出流被冲洗。**
**`fflush` 仅仅刷新用户空间的由 C 库 提供的缓冲。要保证数据被物理地存储到磁盘上，必须也刷新内核缓冲。例如，使用 `sync(2)` 或 `fsync(2)` 。**

### 5.5 打开流
```c
#include <stdio.h>
FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *stream);
```

**参数**

|      mode      |                                   描述                                    |             open(2)对应标志             |
| -------------- | ------------------------------------------------------------------------- | --------------------------------------- |
|     r / rb     |                      **读方式**，流指向文件**开始位置**                       |                O_RDONLY                 |
|     w / wb     |   **写**，将文件长度截断至0，如果不存在，则创建文件，流指向文件**开始位置**   | O_WRONLY &#124; O_CREAT &#124; O_TRUNC  |
|     a / ab     |         **写**，追加（文件尾），不存在则创建文件，流指向**文件末尾**          | O_WRONLY &#124; O_CREAT &#124; O_APPEND |
| r+ / r+b / rb+ |                          **读、写**，流指向开始位置                           |                 O_RDWR                  |
| w+ / w+b / wb+ |   **读、写**，将文件长度截断至0，如果不存在，则创建文件，流指向**开始位置**   |  O_RDWR &#124; O_CREAT &#124; O_TRUNC   |
| a+ / a+b / ab+ | **读、写**， 如果文件不存在，则创建文件，**读文件从开始读，写文件则追加至文件尾** |  O_RDWR &#124; O_CREAT &#124; O_APPEND  |

打开**二进制文件**时使用 `b` ，在区分**文本文件**和**二进制文件**的系统中可以使得文件以合适的方式打开。但是在 `UNIX` 和 `Linux` 等遵循 `POSIX` 的系统中，对两种文件不进行区分，`b` 都被自动忽略。如果为了良好的可移植，最好加上 `b` 。

**返回值**

- 成功：返回指向文件的 `FILE` 对象指针
- 出错：返回 `NULL`

#### `fopen`

打开 `pathname` 指向的一个文件，并与一个流关联

#### `fdopen`

将一个流关联到**已经存在**的文件描述符 `fd` ，**打开模式（`mode`）必须与文件描述符 fd 的 open 模式相匹配**，流的定为与 `fd` 的偏移量相同，错误和文件结束标记被清除。`w` 或 `w+` 打开的文件也**不被截断**，文件描述符不会被复制(`dup`)，在关闭由 `fdopen` 创建的流时，被关闭文件描述符。对共享内存对象实施 `fdopen` 的结果是未定义的。

**通常用于创建管道和网络通信通道函数返回的描述符**，因为这些特殊的文件不能用标准I/O函数 `fopen` 打开。

#### `freopen`

在一个指定的流上打开一个指定的文件，如果该流已经打开，则先关闭该流；若该流已经定向，则使用 `freopen` 清除该定向。

**一般用于将一个指定的文件打开为一个预定的流：标准输入、输出、错误，即改变与标准文本流 (stderr, stdin, 或 stdout) 相关联的文件**

**注意：**

当以**读和写**类型打开一个文件时，具有以下限制：
- 如果中间没有 fflush、fseek、fsetpos、rewind，则在输出的后面不能直接跟随输入。
- 如果中间没有 fseek、fsetpos、rewind，或者一个输入操作没有到达文件尾端，则在输入操作之后不能直接跟随输出。

**w 或 a 创建文件无法说明文件访问权限，使用权限位集来创建文件：**
- S_IRUSR
- S_IWUSR
- S_IRGRP
- S_IWGRP
- S_IROTH
- S_IWOTH

#### `flose`
```c
#include <stdio.h>
int fclose(FILE *stream);
```

**返回值：**
- 成功：0
- 失败：EOF

冲洗缓冲区中的输出数据。缓冲区中的任何输入数据被丢弃。如果标准I/O库已经为该流自动分配了一个缓冲区，则释放此缓冲区。

当一个进程正常终止时，则所有带未写缓冲数据的标准I/O流都被冲洗，所有打开的标准I/O流都被关闭。

### 5.6 读和写流(非格式化I/O)
**对流进行读写的三种 <u>非格式化</u> I/O：**
- **单字符I/O。** 一次读或写一个字符，如果带缓冲，标准I/O函数处理缓冲
- **每次一行I/O。** 一次读或写一行，使用 fgets 和 fputs 。每行以一个换行符终止。
- **直接I/O。** 一次读或写一个对象（结构）。fread 和 fwrite 函数支持此类型。

#### 单字符I/O

**输入函数**

以下3个函数可用于一次读一个字符
```c
#include <stdio.h>
int fgetc(FILE *stream);
int getc(FILE *stream);
int getchar(void);
int ungetc(int c, FILE *stream);
```

- `getchar` 等同于 `getc(stdin)`

- `getc` 可被实现为宏，`fgetc` 不能实现为宏
  - `getc` 参数不能是表达式，可能被多次计算
  - `fgetc` 一定是一个函数，可以得到地址。允许将 `fgetc` 作为一个参数传给另一个函数
  - `fgetc` 调用时间比 `getc` 长。（函数 > 宏）

- `ungetc` 把读出来的字符，压送（push）回流（**缓冲区**）中。
  - 压回的字符在**读出的顺序与压回时相反**。
  - 一次 **push back** 一个字符。不一定是上次读到的字符。
  - 不能回送 EOF
  - 到达文件尾端时，仍可以会送一个字符。下次读则返回字符，再读 返回 EOF
  - 成功调用则清除流中的文件结束标识符

**返回值**
- **`fgetc/getc/getchar`**
  - 成功：返回字符 `int` 值
  - 失败：`EOF` (**文件尾、出错**)
- **`ungetc`**
  - 成功：返回 `c`
  - 失败：返回`EOF`（出错）

**出错、到达文件尾端，返回同样的值。调用 `ferror` 或 `feof` 区分。**
```c
#include <stdio.h>
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fileno(FILE *stream);
```

- `clearerr` 清除 `FILE` 对象中**出错标志**和**文件结束标志**
- `feof` 测试 `stream` 中文件结束标志，如果已设置，返回**非0**，否则返回0
- `ferror` 测试 `stream` 中的错误标记，如果已设置，返回**非0**，否则返回0
- `fileno` 检测 `stream` ，返回文件描述符的 `int` 值，用于文件指针和文件描述符的转换

**输出函数**
```c
#include <stdio.h>
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
```

- `putchar` 等同于` putc(c,stdout)`
- `putc` 可被实现为宏，`fputc` 不能

### 5.7 每次一行I/O

#### 输入函数
```c
#include <stdio.h>
char *gets(char *s);
char *fgets(char *s, int size, FILE *stream);
```

**返回值**
- 成功：返回 s
- 失败：（到达文件尾端或出错）返回 NULL

**`gets`**

**因为其安全性，现已被摒弃，使用 fgets 代替**
- 从标准输入读取一行
- 不把换行符存入缓冲区（删除换行符）
- 使用 `null('\0')` 替代换行符或 `EOF`

**`fgets`**
- 从指定的流 `stream` 读取数据
- 指定缓冲长度 `size`
- 读到换行符为止，且不超过 `size-1` 个字符
- 读入缓冲区以 `null` 结尾，如果包含换行符超过 `size-1` ，则返回一个不完整的行且以 `null` 结尾，下次调用继续读取该行。

#### 输出函数
```c
#include <stdio.h>
int puts(const char *s);
int fputs(const char *s, FILE *stream);
```

**返回值**
- 成功：非负值
- 失败： `EOF`

**`fputs`**
- 将以 `null` 终止的字符串写到流，`null` 不写出
- 不要求**换行符**作为 `null` 之前的最后一个字节，所以不一定每次输出一行

**`puts`**
- 将 `null` 终止的字符串写到标准输出，`null` 不写出
- 在尾部添加换行符（`'\n'`）写到标准输出
- 建议使用 `fputs` 替代，自己**手动处理换行符**

### 2.8 标准 I/O 的效率

`exit` 函数将会冲洗任何未写的数据，然后打开关闭的流。

- **getcputc.c**
- **fgetsfputs.c**

每次一行 I/O 速度更快，利用 `memccpy` 函数实现，`memccpy` 用汇编语言编写

系统调用比普通的函数调用花费更多时间。例如：2亿次 `read` 时间（2亿次系统调用） **远远大于** 2亿次 `fgetc` 时间（25224次系统调用）。

标准 I/O 库与直接调用 read 和 write 函数相比并不慢很多，对于大多数比较复杂的应用程序，最重要的用户 CUP 时间是由应用本身的各种处理消耗的，而不是标准I/O。

### 5.9 二进制 I/O
一次读或写一个完整的结构，或者包含 `null` 字节的数据，因为 `fgets` 遇到 `null` 字节就停止。例如，输入的数据中有换行符或者 `null` ，则 `fgets` 不能正确处理。

```c
#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

`fread` 和 `fwrite` 返回读或写的对象数（数据条数 `items` ）。如果出错或者到达文件尾端，此数字可以少于 `nmemb` ，此时利用 `ferror` 或 `feof` 判断具体情况。对于写，如果返回值小于 `nmemb` ，则出错。

二进制 I/O ，**用于同一系统上读写的数据**，异构系统之间的数据，两函数通常不能正常工作。

- 不同系统中，结构中的偏移量可能不同，成员包装方式，对齐方式都可能不同
- 不同系统用来存储多字节整数和浮点值的二进制格式不同

### 5.10 定位流
有三种方法定位标准 I/O 流。
1. `ftell` 和 `fseek` 函数。假定文件位置存放在**长整型 long**中。
2. `ftello` 和 `fseeko` 函数。 `off_t` 替代长整型。
3. `fgetpos` 和 `fsetpos` 函数。使用抽象数据类型 `fpos_t` 记录文件位置。

需要**移植到非 UNIX 系统**上运行的程序应当使用 `fgetpos` 和 `fsetpos` 。

```c
#include <stdio.h>
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
```

对于二进制文件，文件位置指示器从文件起始位置开始度量，并以字节为度量单位。 `ftell` 用于二进制文件时，返回这种字节位置。 `fseek` 支持三种偏移方式（见3.6节，`lseek`），某些要求二进制长度是某个常量的整数倍，不足的末尾用 `0` 填充。

对于文本文件（主要在非 `UNIX` 系统中），文件当前位置不能以简单的字节偏移量来度量。 `fseek` 的参数 `whence` 必须是 `SEEK_SET` ，且 `offset` 取值为 `0`（文件起始位置），或者 `ftell` 返回的值。

使用 `rewind` 函数也可将一个流设置到文件的起始位置。可用于刷新缓冲区。

```c
#include <stdio.h>
int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);
```

```c
#include <stdio.h>
int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, const fpos_t *pos);
```

`fgetpos` 和 `fsetpos` 两个函数是 `ISO C` 标准引入的。 `fgetpos` 将文件位置指示器的当前值存入 `pos` 指向的对象中，以后调用 `fsetpos` 时，可以使用此值将流重新定位至该位置。

### 5.11 格式化 I/O
#### 1. 格式化输出
```c
#include <stdio.h>
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
```

- `printf` 将格式化数据写到标准输出。
- `fprintf` 将数据写到指定的流。
- `dprintf` 将数据写到指定的文件描述符。不需要用 `fdopen` 将文件描述符转换为文件指针（参数`fd`）
- `sprintf` 将数据写入 `str` 数组中，并在该数组尾端自动添加一个 `null` 字节，但返回值中不包含该字符 `null` ，存在缓冲区溢出隐患。
- `snprintf` 相比于 `sprintf` 安全性更高，为解决缓冲区溢出，超过缓冲区尾端的字符都被丢弃。返回值不包括 `null` 字节，若返回值小于 `size` 的**正值**，那么没有截断输出；若发生了一个错误的编码，返回负值。

**`printf` 族变体函数，可变参数列表（...）替换成为 `arg`**

```c
#include <stdarg.h>
int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vdprintf(int fd, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
```

#### 2. 格式化输入
```c
#include <stdio.h>
int scanf(const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
```

`scanf` 族变体函数，由 `<stdarg.h>` 头文件说明
```c
#include <stdarg.h>
int vscanf(const char *format, va_list ap);
int vsscanf(const char *str, const char *format, va_list ap);
int vfscanf(FILE *stream, const char *format, va_list ap);
```

### 5.12 实现细节
- buf.c
```shell

```

### 5.13 临时文件
ISO C 标准 I/O 库提供了两个函数以帮助创建临时文件。
```c
#include <stdio.h>
char *tmpnam(char *s);
FILE *tmpfile(void);
                    # 返回值：成功，返回文件指针；出错，返回 NULL
```

- `tmpnam`

函数产生一个与现有文件名不同的一个有效路径名字符串。每次调用它时，都产生一个不同的路劲名，最多调用次数是 `TMP_MAX` 。 `TMP_MAX` 定义在 `<stdio.h>` 中。在 SUS(single UNIX Specification) 中已被弃用。

若 `s` 是 `null` ，则所产生的的路径名存放在一个静态区中，指向该静态区的指针作为函数值返回。后续调用 `tmpnam` 时，重写该静态区（应当保存路径名的副本，而不是指针的副本）。

若 `s` 不是 `null` ，则认为它指向长度至少是 `L_tmpnam` 个字符的数组（`L_tmpnam` 定义在头文件 `<stdio.h>` 中），路径名存放在该数组中，`s` 作为函数值返回。

- `tmpfile`

创建一个临时二进制文件（类型 `wb+`），在关闭该文件或者程序结束时自动删除这种文件。注意 `UNIX` 对二进制文件不进行特殊区分。

- `tempfiles.c`
```shell
$ ./a
/tmp/filewDXkUR
/tmp/fileEr3cfh
one line of output
```

SUS 为处理临时文件定义了另外两个函数 `mkdtemp` 和 `mkstemp`
```c
#include <stdlib.h>
char *mkdtemp(char *template);
            # 返回值：成功，返回目录指针；错误，返回 NULL
int mkstemp(char *template);
            # 返回值：成功，返回文件描述符；错误，返回-1
```

- `mkdtemp`

函数创建一个唯一名字的目录，名字通过 `template` 字符串选择。这个字符串是后6位设置为 XXXXXX 的路径名。函数将这些占位符替换成不同的字符来构建一个路径名。如果成功，函数将修改 `template` 字符串反映临时文件的名字。

- `mkstemp`

函数创建一个唯一名字的文件，**临时文件不会自动删除**。如果希望从文件系统命名空间中删除该文件，必须自己对它解除链接。

**注意：**

1. **因为要修改 `template` 的值，所以 `template` 应是字符串数组，而不是字符串常量。** 数组在栈上分配，字符串常量存放在只读段，试图修改时，出现段错误。例如：
```c
# 正确方式
char good_template[] = "/tmp/dirXXXXXX";
# 错误
char * bad_template = "/tmp/dirXXXXXX";
```

- `mkstemp`
```shell
$./a
trying to creat first temp file...
temp name = /tmp/dirCxeE0v
file exists
trying to creat second temp file...
段错误 (核心已转储)
```

2. 使用 `tmpnam` 和 `tempnam` 时，在返回路径名和用该路径名创建文件之间存在时间窗口，在这个窗口中，另一进程可以用相同的名字创建文件。因此应该使用 `tmpfile` 和 `mkstemp` 函数。

