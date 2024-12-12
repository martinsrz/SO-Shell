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
#include "directoriesList.h"
#include "list.h"

#define MAXVAR 2048

void Cmd_fork();
void getUid();
void uidSetId(char *id);
void uidSetUsername(char *username);
int BuscarVariable(char *var, char *e[]);
int CambiarVariable(char * var, char * valor, char *e[]);
char *Ejecutable(char *s, tListD directoriesList);
int Execpve(char *tr[], char **NewEnv, int *pprio, tListD directoriesList);


#endif