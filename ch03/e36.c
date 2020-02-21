#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <apueerror.h>

int main(void)
{
	int fd;
	char buf[20];
	fd  = open("3.6.file", O_RDWR|O_APPEND);
	if(fd == -1)
	{
		fd  = open("3.6.file", O_RDWR|O_APPEND|O_CREAT);
		write(fd, "hello", 5);
	}
	
	if(lseek(fd, 2, SEEK_SET) == -1)
		err_sys("lseek error");

	read(fd, buf, 5);
	printf("lseek read: %s\n", buf);

	if(lseek(fd, 0, SEEK_SET) == -1)
		err_sys("lseek error");

	write(fd, "world!", 6);
	
	if(lseek(fd, 0, SEEK_SET) == -1)
		err_sys("lseek error");
	
	read(fd,buf,20);
	
	printf("lseek write: %s\n", buf);
}

