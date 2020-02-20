OBJ=canseek seekhole mycat fileflag
CC=gcc
CFLAGS=-lapue -Wall
LIBAPUE=/usr/lib/libapue.a
all:$(OBJ)
%:  %.c $(LIBAPUE)
	$(CC) $@.c -o $@ $(CFLAGS)
.PHONY:clean
clean:
	rm -f $(OBJ) *.o
