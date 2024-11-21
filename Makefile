shell.out: p1.c list.o memoryList.o

	gcc -g -Wall -o shell.out p1.c list.o memoryList.o

list.o: list.h list.c

	gcc -c list.h list.c

memoryList.o: memoryList.h memoryList.c

	gcc -c memoryList.h memoryList.c

valgrind:

	gcc -g -O0 -Wall -o shell.out p1.c list.o memoryList.o
	valgrind -s --leak-check=full --show-reachable=yes ./shell.out
