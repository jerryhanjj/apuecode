#include <stdio.h>
#include <string.h>
#include <apueerror.h>

#define BSZ 48

int main(void)
{
	
	FILE *fp;
	char buf[BSZ];

	memset(buf, 'a', BSZ-2);

	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';

	if((fp = fmemopen(buf, BSZ, "w+")) == NULL)
		err_sys("fmemopen error");

	printf("initial buffer contents: %s\n", buf);
	printf("buf+0 = %s\n", buf);
	printf("buf+1 = %s\n\n", buf+1);
	
	fprintf(fp, "hello, world");
	printf("before flush: %s\n", buf);

	fflush(fp);

	printf("after fflush: %s\n", buf);
	printf("buf+12 = %s\n", buf+12);
	printf("buf+13 = %s\n", buf+13);
	printf("len of string in buf = %ld\n\n", (long)strlen(buf));
	
	memset(buf, 'b', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	fprintf(fp, "hello, world");
	fseek(fp, 0, SEEK_SET);
	printf("after fseek: %s\n", buf);
	printf("buf+24 = %s\n", buf+24);
	printf("buf+25 = %s\n", buf+25);
	printf("len of string in buf = %ld\n\n", (long)strlen(buf));

	memset(buf, 'c', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	fprintf(fp, "hello, world! I'm Daniel.");
	fclose(fp);
	printf("after flose: %s\n", buf);
	printf("len of string in buf = %ld\n\n", (long)strlen(buf));
	
	return 0;
}
