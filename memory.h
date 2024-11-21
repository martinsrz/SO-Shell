#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <errno.h>
#include <sys/shm.h>
#include "memoryList.h"

#define TAMANO 2048

void Recursiva (int n);
void MemoryFuncs();
void Do_pmap ();

#endif