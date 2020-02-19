## 《UNIX环境高级编程》源码编译
### 下载
源码下载：[www.apuebook.com](http://www.apuebook.com/)

### 安装 `libbsd-dev`
Ubuntu：
```shell
sudo apt-get install libbsd-dev
```

### 编译
```shell
cd apue.3e
make
```

### 环境搭建
移动`头文件`和`静态链接库`到系统默认搜索路径中，方便以后在代码中引用。
添加`apue.h`和`libapue.a`
```shell
sudo cp include/apue.h /usr/include
sudo cp lib/libapue.a /usr/lib
sudo cp lib/libapue.a /usr/local/lib
```

如果在学习中需要单独使用作者的错误输出函数（duck不必），则要添加函数文件到系统头文件目录中。  
新建`apueerror.h`，复制`apue.3e/lib/error.c`中的内容到到新建的文件中。
```shell
sudo cp apue.3e/lib/error.c apueerror.h
```

将`apueerror.h`添加到系统库中，以后便可作为系统头文件单独引用
```shell
sudo cp apueerror.h /usr/include
```

### 编译第一个例子 `ls`
编译时需要链接静态链接库`-lapue`
```shell
gcc ls1.c -o 1 -lapue
./1 .     # 列出当前目录的文件
```


### 作者述
Read the file called DISCLAIMER.

On Freebsd, type "gmake".
On other platforms, type "make" (as long as this is gnu make).

For FAQs, updated source code, and the lost chapter, see http://www.apuebook.com.
Please direct questions, suggestions, and bug reports to sar@apuebook.com.

Steve Rago
January 2013
