// Martín Tubio Suaŕez
// Diego Candal Varela

#ifndef MEMORYLIST_H
#define MEMORYLIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define COMMAND_LEN 256
#define LNULL NULL
typedef char command[COMMAND_LEN];

typedef struct tNodeM *tPosM;

typedef struct tItemM {
    void *memoryAdress;
    int size;
    struct tm *time;
    command mode;
    char type;
    int key;
    int fd;
    command name;
} tItemM;

typedef struct tNodeM {
    tItemM data;
    tPosM next;
} tNodeM;

typedef tPosM tListM;

void createEmptyListM(tListM *L);

bool insertItemM(tItemM d, tPosM p, tListM *L);

void updateItemM(tItemM d, tPosM p, tListM *L);

void deleteAtPositionM(tPosM p, tListM *L);

void deleteListM(tListM *L);

bool isEmptyListM(tListM L);

tPosM findItemM(command d, tListM L);

tPosM findPositionM(int n, tListM L);

tItemM getItemM(tPosM p, tListM L);

tPosM firstM(tListM L);

tPosM lastM(tListM L);

tPosM nextM(tPosM p, tListM L);

tPosM previousM(tPosM p, tListM L);

int countListM(tListM L);

#endif