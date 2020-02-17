#include <apue.h>

int main(void)
{
	printf("hello world from pid %ld\n", (long)getpid());
	exit(0);
}
