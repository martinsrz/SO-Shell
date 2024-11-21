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
#include "list.h"

#define TAMANO 2048

void *MapearFichero(char *fichero, int protection, tListM *memoryList, tList *openFiles);
void *ObtenerMemoriaShmget (key_t clave, size_t tam, tListM *memoryList);
void do_AllocateMalloc(size_t size, tListM *memoryList);
void do_AllocateMmap(char *file, char *perms, tListM *memoryList, tList *openFiles);
void do_AllocateCreateshared (char *cl, char *n, tListM *memoryList);
void do_AllocateShared(char *clv, tListM *memoryList);
void do_DeallocateMalloc(size_t size, tListM *memoryList);
void do_DeallocateDelkey (char *key);
void Recursiva (int n);
void MemoryFuncs();
void MemoryVars();
void MemoryBlocks(char *type, tListM memoryList);
void Do_pmap ();

#endif