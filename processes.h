#ifndef PROCESSES_H
#define PROCESSES_H

#include <pwd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
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
#include "list.h"
#include "memoryList.h"
#include "directoriesList.h"
#include "processesList.h"

#define MAXVAR 2048

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
void Cmd_fork(tListP *processesList);
void showSearchDirectories(tListD processesList);
void searchAddDir(char *dir, tListD *directoriesList);
void searchDelDir(char *dir, tListD *directoriesList);
void searchPath(tListD *directoriesL);
void exec(char *tr[], int nArgs, tListD directoriesList, tListM *envList);
void execFg(char *tr[], int nArgs, tListD directoriesList, tListM *envList);
void execFgprio(char *tr[], int nArgs, tListD directoriesList, tListM *envList);
void execBack(char *tr[], int nArgs, tListD directoriesList, tListM *envList, tListP *processesList);
void execBackpri(char *tr[], int nArgs, tListD directoriesList, tListM *envList, tListP *processesList);
void listjobs(tListP *processesList);
void delterm(tListP *processesList);
void delsig(tListP *processesList);
int BuscarVariable(char *var, char *e[]);
int CambiarVariable(char * var, char * valor, char *e[], tListM *envList);
int SustituirVariable(char * var, char * var2, char * valor, char *e[], tListM *envList);
char *Ejecutable(char *s, tListD directoriesList);
int Execpve(char *tr[], char **NewEnv, int *pprio, tListD directoriesList);
int Execforeground(char *tr[], char **NewEnv, int *pprio, tListD directoriesList);
int Execbackground(char *tr[], char **NewEnv, int *pprio, tListD directoriesList, tListP *processesList);


#endif