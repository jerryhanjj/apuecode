#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <apueerror.h>

void display_fd_fl(int *fd)
{
	int i = 0;
	int fd_fl;
	for(i; i<3; i++)
	{
		if((fd_fl = fcntl(fd[i], F_GETFD)) == -1)
		{
			err_sys("get F_GETFD error");
		}
		printf("fd %d : FD_CLOEXEC=%d ", i, fd_fl);
		if((fd_fl = fcntl(fd[i], F_GETFL)) == -1)
		{
			err_sys("get F_GETFL error");
		}
		printf("mode=%d\n", fd_fl);
	}
}
int main(void)
{
	int fd[3];

	if((fd[0] = open("a.file", O_RDWR|O_CREAT)) == -1)
		err_sys("fd[0] open error");
	fd[1] = dup(fd[0]);
	if((fd[2] = open("a.file", O_RDONLY|O_CREAT)) == -1)
		err_sys("fd[2] open error");

	display_fd_fl(fd);

	printf("--------修改fd[0] FD-------\n");
	fcntl(fd[0], F_SETFD, 1);
	display_fd_fl(fd);
	printf("--------修改fd[2] FD-------\n");
	fcntl(fd[2], F_SETFD, 1);
	display_fd_fl(fd);
	printf("--------修改fd[0] FL-------\n");
	fcntl(fd[0], F_SETFL, O_APPEND);
	display_fd_fl(fd);
	printf("--------修改fd[2] FL-------\n");
	fcntl(fd[2], F_SETFL, O_APPEND);
	display_fd_fl(fd);
}

