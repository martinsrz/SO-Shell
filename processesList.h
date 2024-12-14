// Martín Tubio Suaŕez
// Diego Candal Varela

#ifndef PROCESSESLIST_H
#define PROCESSESLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define COMMAND_LEN 256
#define LNULL NULL
typedef char command[COMMAND_LEN];

typedef struct tNodeP *tPosP;

typedef struct tItemP {
    pid_t pid;
    command time;
    command status;
    command cmnd;
    command user;
    int priority;
    command signal;
} tItemP;

typedef struct tNodeP {
    tItemP data;
    tPosP next;
} tNodeP;

typedef tPosP tListP;

struct SEN {
    char *nombre;
    int senal;
};

int ValorSenal(char * sen);
char *NombreSenal(int sen);

void createEmptyListP(tListP *L);

bool insertItemP(tItemP d, tPosP p, tListP *L);

void updateItemP(tItemP d, tPosP p, tListP *L);

void deleteAtPositionP(tPosP p, tListP *L);

void deleteListP(tListP *L);

bool isEmptyListP(tListP L);

tPosP findItemP(pid_t pid, tListP L);

tPosP findPositionP(int n, tListP L);

tItemP getItemP(tPosP p, tListP L);

tPosP firstP(tListP L);

tPosP lastP(tListP L);

tPosP nextP(tPosP p, tListP L);

tPosP previousP(tPosP p, tListP L);

int countListP(tListP L);

void refreshItemP(tPosP p, tListP *L);

#endif