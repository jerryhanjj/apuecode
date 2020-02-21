#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "openmax.c"
#include <apueerror.h>

long open_max();

int my_dup(int fd1, int fd2)
{
	int fd[fd2];
	int fd_temp;
	int i = 0;

	if(fd2 >= (open_max()))
	{
		printf("error: fd2 beyond OPEN_MAX\n");
		printf("usage: fd2 < %ld\n", open_max());
		exit(1);
	}

	if(fd1 == fd2)
	{
		return fd1;
	}

	close(fd2);
	
	while((fd_temp = dup(fd1)) != fd2)
	{
		fd[i] = fd_temp;
		i++;
	}
	
	for(i; i>=0; i--)
	{
		printf("close fd: %d\n", fd[i]);
		close(fd[i]);		
	}

	return fd_temp;
}

int main(int argc, char *argv[])
{
	int fd;

	if(argc != 2)
	{
		err_quit("Usage: <fd>");
	}

	fd = my_dup(STDOUT_FILENO, atoi(argv[1]));
	write(fd, "my dup test...\n", 14);
	exit(0);
}
