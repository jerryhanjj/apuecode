#include <unistd.h>

int main(void)
{
	write(STDOUT_FILENO, "out\n", 4);
	write(STDERR_FILENO, "err\n", 4);
	return 0;
}
