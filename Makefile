shell.out: p3.c list.o memoryList.o memory.o processesList.o processes.o directoriesList.o

	gcc -g -Wall -o shell.out p3.c list.o memoryList.o memory.o processesList.o processes.o directoriesList.o

list.o: list.h list.c

	gcc -c list.h list.c

memoryList.o: memoryList.h memoryList.c

	gcc -c memoryList.h memoryList.c

memory.o: memory.h memory.c

	gcc -c memory.h memory.c

processesList.o: processesList.h processesList.c

	gcc -c processesList.h processesList.c

processes.o: processes.h processes.c

	gcc -c processes.h processes.c

directoriesList.o: directoriesList.h directoriesList.c

	gcc -c directoriesList.h directoriesList.c

valgrind:

	gcc -g -O0 -Wall -o shell.out p3.c list.o memoryList.o memory.o processesList.o processes.o directoriesList.o 
	valgrind -s --leak-check=full --show-reachable=yes ./shell.out
