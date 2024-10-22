shell.out: p0.c list.o

	gcc -g -Wall -o shell.out p0.c list.o

list.o: list.h list.c

	gcc -c list.o list.c

valgrind:

	gcc -g -O0 -Wall -o shell.out p0.c list.o
	valgrind -s --leak-check=full --show-reachable=yes ./shell.out
