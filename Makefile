shell.out: p0.c list.o

	gcc -g -Wall -o shell.out p0.c list.o

list.o: list.h list.c

	gcc -c list.o list.c
