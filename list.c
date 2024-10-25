// Martín Tubio Suaŕez
// Diego Candal Varela

#include "list.h"

bool createNode(tPos *p)
{
    *p = malloc(sizeof(struct tNode));

    return *p != LNULL;
}

void createEmptyList(tList *L)
{
    *L = LNULL;
}

bool insertItem(tItem d, tPos p, tList *L)
{
  	tPos q, r;

 	if (!createNode(&q)) {
      	return false;
  	} else {
  	    q->data = d;
  	    q->next = LNULL;
  	    if (*L == LNULL) {
	        *L = q;
  		} else if (p == LNULL) {
          	for (r = *L; r->next != LNULL; r = r->next);
          	r->next = q;
      	} else if (p == *L) {
          	q->next = p;
          	*L = q;
      	} else {
          	q->data = p->data;
          	p->data = d;
          	q->next = p->next;
          	p->next = q;
      	}
      	return true;
  	}
}

void updateItem(tItem d, tPos p, tList *L)
{
	p->data = d;
}

void deleteAtPosition(tPos p, tList *L)
{
  	tPos q;

  	if (p == *L) {
  		*L = p->next;
  	} else if (p->next == LNULL) {
    	for (q = *L; q->next != p; q = q->next);
		q->next = LNULL;
    } else {
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}

void deleteList(tList *L)
{
    tPos p;
			
	while (*L != LNULL)
	{
		p = *L;
		*L = p->next;
		free(p);
	}
}

bool isEmptyList(tList L)
{
  return (L == LNULL);
}

tPos findItem(command d, tList L)
{
    tPos p;
    for (p = L; (p != LNULL) && (strcmp(p->data.command, d) != 0); p = p->next);
    return p;
}

tPos findPosition(int n, tList L)
{
	tPos pos = L;
	int count = 0;

	while (pos != LNULL && count < n)
	{
		pos = pos->next;
		count++;
	}

	if (count == n)
	{
		return pos;
	}
	else
	{
		return LNULL;
	}
}

tItem getItem(tPos p, tList L)
{
    return p->data;
}

tPos first(tList L)
{
    return L;
}

tPos last(tList L)
{
    tPos p;
    for (p = L; p->next != LNULL; p = p->next);
    return p;
}

tPos next(tPos p, tList L)
{
    return p->next;
}

tPos previous(tPos p, tList L)
{
    tPos q;

    if (p == L) {
        return LNULL;
    } else {
        for (q = L; q->next != p; q = q->next);
        return q;
    }
}

tPos findDescriptor(int df, tList L)
{
	tPos p;

	p = first(L);
	if (p == LNULL)
		return p;

	while (p->data.fileDescriptor != df && p->next != NULL)
	{
		p = p->next;
	}

	if (df == p->data.fileDescriptor)
	{
		return p;
	}
	else return LNULL;
}


int countList(tList L)
{
	int count = 0;
	tPos p = first(L);

	for (int i = 0; p != NULL; p = p->next)
	{
		count++;
	}

	return count;
}

