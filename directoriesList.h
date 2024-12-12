// Martín Tubio Suaŕez
// Diego Candal Varela

#ifndef DIRECTORIESLIST_H
#define DIRECTORIESLIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define COMMAND_LEN 256
#define LNULL NULL
typedef char command[COMMAND_LEN];

typedef struct tNodeD *tPosD;

typedef struct tItemD {
    command directory;
} tItemD;

typedef struct tNodeD {
    tItemD data;
    tPosD next;
} tNodeD;

typedef tPosD tListD;

void createEmptyListD(tListD *L);

bool insertItemD(tItemD d, tPosD p, tListD *L);

void updateItemD(tItemD d, tPosD p, tListD *L);

void deleteAtPositionD(tPosD p, tListD *L);

void deleteListD(tListD *L);

bool isEmptyListD(tListD L);

tPosD findItemD(command d, tListD L);

tPosD findPositionD(int n, tListD L);

tItemD getItemD(tPosD p, tListD L);

tPosD firstD(tListD L);

tPosD lastD(tListD L);

tPosD nextD(tPosD p, tListD L);

tPosD previousD(tPosD p, tListD L);

int countListD(tListD L);

#endif