#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <apueerror.h>
#include "setfcntl.c"

void set_fl(int fd, int flags);

#define BUFFSIZE 4096

int main(void)
{
	int n;
	char buf[BUFFSIZE];

	//set_fl(STDOUT_FILENO, O_SYNC);
	
	int fd = open("2.dat", O_WRONLY|O_SYNC|O_CREAT);
	
	while((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
	{
		if((write(fd/*STDOUT_FILENO*/, buf, n)) != n)		// 使用open时，STDOUT_FILENO 改成 fd
		{
			err_sys("write error");
		}
	}

	if(n < 0)
	{
		err_sys("read error");
	}

	exit(0);
}
