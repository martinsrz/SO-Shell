// Martín Tubio Suaŕez
// Diego Candal Varela

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define COMMAND_LEN 256
#define LNULL NULL
#define MAX_LENGTH 256
typedef char command[COMMAND_LEN];

typedef struct tNode *tPos;

typedef struct tItem {
    command command;
    int fileDescriptor;
    int mode;
} tItem;

typedef struct tNode {
  tItem data;
  tPos next;
} tNode;

typedef tPos tList;

void createEmptyList(tList *L);

bool insertItem(tItem d, tPos p, tList *L);

void updateItem(tItem d, tPos p, tList *L);

void deleteAtPosition(tPos p, tList *L);

void deleteList(tList *L);

bool isEmptyList(tList L);

tPos findItem(command d, tList L);

tPos findPosition(int n, tList L);

tItem getItem(tPos p, tList L);

tPos first(tList L);

tPos last(tList L);

tPos next(tPos p, tList L);

tPos previous(tPos p, tList L);

tPos findDescriptor(int df, tList L);

int countList(tList L);

#endif