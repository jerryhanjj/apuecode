#include <apue.h>

int main(void)
{
	if(lseek(STDIN_FILENO, 0, SEEK_CUR) < 0)
	{
		printf("can't seek\n");
	}else
	{
		printf("seek ok\n");	
	}

	exit(0);
}
