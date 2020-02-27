OBJ=getcputc fgetsfputs buf mkstemp tempfiles
CC=gcc
CFLAGS=-lapue -Wall
all:$(OBJ)
% :%.c
	$(CC) $@.c -o $@ $(CFLAGS)
.PHONY:clean
clean:
	rm -rf $(OBJ) *.o

