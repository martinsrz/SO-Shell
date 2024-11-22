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

void *cadtop(char *str);
void *MapearFichero(char *fichero, int protection, tListM *memoryList, tList *openFiles);
void *ObtenerMemoriaShmget (key_t clave, size_t tam, tListM *memoryList);
ssize_t LeerFichero (char *f, void *p, size_t cont);
ssize_t EscribirFichero(char *f, void *p, size_t cont, int overwrite);
ssize_t LeerDf(int df, void *p, size_t cont);
ssize_t EscribirDf(int fd, void *p, size_t cont);
void do_AllocateMalloc(size_t size, tListM *memoryList);
void do_AllocateMmap(char *file, char *perms, tListM *memoryList, tList *openFiles);
void do_AllocateCreateshared (char *cl, char *n, tListM *memoryList);
void do_AllocateShared(char *clv, tListM *memoryList);
void do_DeallocateMalloc(size_t size, tListM *memoryList);
void do_DeallocateMmap(char *file, tListM *memoryList, tList *openFiles);
void do_DeallocateShared(char *clv, tListM *memoryList);
void do_DeallocateDelkey (char *key);
void do_Deallocate(char *address, tListM *memoryList, tList *openFiles);
void LlenarMemoria(void *p, size_t cont, unsigned char byte);
void MemoryFuncs();
void MemoryVars();
void MemoryBlocks(char *type, tListM memoryList);
void Do_pmap ();
void Recursiva (int n);

#endif