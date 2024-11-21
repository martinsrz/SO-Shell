// Martín Tubio Suaŕez
// Diego Candal Varela

#include "memoryList.h"

bool createNodeM(tPosM *p)
{
    *p = malloc(sizeof(struct tNodeM));

    return *p != LNULL;
}

void createEmptyListM(tListM *L)
{
    *L = LNULL;
}

bool insertItemM(tItemM d, tPosM p, tListM *L)
{
  	tPosM q, r;

 	if (!createNodeM(&q)) {
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

void updateItemM(tItemM d, tPosM p, tListM *L)
{
	p->data = d;
}

void deleteAtPositionM(tPosM p, tListM *L)
{
  	tPosM q;

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

void deleteListM(tListM *L)
{
    tPosM p;

	while (*L != LNULL)
	{
		p = *L;
		*L = p->next;
		free(p);
	}
}

bool isEmptyListM(tListM L)
{
  return (L == LNULL);
}

tPosM findItemM(command d, tListM L)
{
    tPosM p;
    for (p = L; (p != LNULL) && (strcmp(p->data.memoryAdress, d) != 0); p = p->next);
    return p;
}

tPosM findPositionM(int n, tListM L)
{
	tPosM pos = L;
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

tItemM getItemM(tPosM p, tListM L)
{
    return p->data;
}

tPosM firstM(tListM L)
{
    return L;
}

tPosM lastM(tListM L)
{
    tPosM p;
    for (p = L; p->next != LNULL; p = p->next);
    return p;
}

tPosM nextM(tPosM p, tListM L)
{
    return p->next;
}

tPosM previousM(tPosM p, tListM L)
{
    tPosM q;

    if (p == L) {
        return LNULL;
    } else {
        for (q = L; q->next != p; q = q->next);
        return q;
    }
}

int countListM(tListM L)
{
	int count = 0;
	tPosM p = firstM(L);

	for (int i = 0; p != NULL; p = p->next)
	{
		count++;
	}

	return count;
}

