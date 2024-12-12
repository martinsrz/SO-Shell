#ifndef PROCESSES_H
#define PROCESSES_H

#include <pwd.h>
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
#include <sys/resource.h>
#include "processesList.h"
#include "directoriesList.h"
#include "list.h"
#include "memoryList.h"

#define MAXVAR 2048

void Cmd_fork(tListP *processesList);
void getUid();
void uidSetId(char *id);
void uidSetUsername(char *username);
void printArg3(char *var, char *arg3[]);
void printEnv(char *var);
void printGetenv(char *var);
void changevar(char *tr0, char *tr1, char *tr2, char *arg3[], tListM *envList);
void subsvar(char *tr0, char *tr1, char *tr2, char *tr3, char *arg3[], tListM *envList);
void environArg3(char *arg3[]);
void environEnv();
void environAddr(char *arg3[]);
int BuscarVariable(char *var, char *e[]);
int CambiarVariable(char * var, char * valor, char *e[], tListM *envList);
int SustituirVariable(char * var, char * var2, char * valor, char *e[], tListM *envList);
char *Ejecutable(char *s, tListD directoriesList);
int Execpve(char *tr[], char **NewEnv, int *pprio, tListD directoriesList);


#endif