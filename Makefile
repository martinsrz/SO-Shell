shell.out: p1.c list.o memoryList.o memory.o

	gcc -g -Wall -o shell.out p1.c list.o memoryList.o memory.o

list.o: list.h list.c

	gcc -c list.h list.c

memoryList.o: memoryList.h memoryList.c

	gcc -c memoryList.h memoryList.c

memory.o: memory.h memory.c

	gcc -c memory.h memory.c

valgrind:

	gcc -g -O0 -Wall -o shell.out p1.c list.o memoryList.o memory.o
	valgrind -s --leak-check=full --show-reachable=yes ./shell.out
