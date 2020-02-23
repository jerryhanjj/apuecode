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
- 参数为负：fwide 将试图指定流是**字节定向**
- 参数为正：fwide 将试图指定流是**宽定向**的
- 参数为 0：fwide 不设置流的定向，返回标识该流定向的值

**返回值：**
- 宽定向：“+”
- 字节定向： “-”
- 未定向： 0

`fiwde` 不改变已定向流的定向，且无出错返回。在调用前清除 `errno`，返回值时检查 `errno` 的值。

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

**目的：**  减少使用 read 和 write 调用的次数，自动对每个 I/O 流进行缓冲管理。

#### 全缓冲

**标准 I/O** 填满缓冲区后才进行**实际的 I/O 操作**。磁盘上的文件就是这样。流第一次进行I/O操作时使用 `malloc` 获得缓冲区。
冲洗（flush）：缓冲区的写操作。缓冲区可由程序自动冲洗，也可调用 `fflush` 冲洗一个流。

**flush两种含义：**
1. 将缓冲区内容写到磁盘
2. 清除缓冲区中数据

#### 行缓冲
在输入和输出中遇到换行符，才执行I/O操作，比如 fputc 一次输出一个字符，但只有在写了一行之后才进行实际I/O操作。终端通常使用航缓冲

**限制：**
1. 每一行缓冲区长度固定，缓冲区满，**没有换行符**也进行I/O操作
2. 从不带缓冲的流，或者行缓冲的流获取数据，**冲洗所有行缓冲输出流**

#### 不带缓冲
标准I/O库不对字符进行缓冲存储。例如，用 fputs 写15个字符到不带缓冲流中，字符立即输出。可能使用 write 直接写入相关文件中。
stderr 通常不带缓冲。

**通常情况：**
- stderr 是不带缓冲的
- 指向终端设备的流，是行缓冲，否则是全缓冲

#### 更改缓冲类型
```c
#include <stdio.h>
void setbuf(FILE *stream, char *buf);
int setvbuf(FILE *stream, char *buf, int mode , size_t size);
```

- setbuf

关闭或打开缓冲机制。带缓冲（全缓冲，如果终端设备，可以为行缓冲）时，`buf` 指向长度为 `BUFSIZ` 的缓冲区；关闭缓冲，`buf` 设置为 `NULL`。

- setvbuf
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

缓冲区的一部分还用来存放**实现**自己的管理操作信息，所以存放在缓冲区的实际字节数少于 `size`。一般系统将自动选择缓冲区大小。

#### 强制冲洗一个流
```c
#include <stdio.h>
int fflush(FILE *stream);
```
**此函数使流所有未写的数据传送至内核，作为例外，如果 fp 是 NULL，则将导致所有的输出流被冲洗。**

