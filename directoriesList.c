// Martín Tubio Suaŕez
// Diego Candal Varela

#include "directoriesList.h"

bool createNodeD(tPosD *p)
{
    *p = malloc(sizeof(struct tNodeD));

    return *p != LNULL;
}

void createEmptyListD(tListD *L)
{
    *L = LNULL;
}

bool insertItemD(tItemD d, tPosD p, tListD *L)
{
  	tPosD q, r;

 	if (!createNodeD(&q)) {
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

void updateItemD(tItemD d, tPosD p, tListD *L)
{
	p->data = d;
}

void deleteAtPositionD(tPosD p, tListD *L)
{
  	tPosD q;

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

void deleteListD(tListD *L)
{
    tPosD p;

	while (*L != LNULL)
	{
		p = *L;
		*L = p->next;
		free(p);
	}
}

bool isEmptyListD(tListD L)
{
  return (L == LNULL);
}

tPosD findItemD(command d, tListD L)
{
	tPosD p;
	for (p = L; (p != LNULL) && (strcmp(p->data.directory, d) != 0); p = p->next);
	if (p != NULL &&
		(strcmp(p->data.directory, d) == 0))
		return p;
	else
		return NULL;
}

tPosD findPositionD(int n, tListD L)
{
	tPosD pos = L;
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

tItemD getItemD(tPosD p, tListD L)
{
    return p->data;
}

tPosD firstD(tListD L)
{
    return L;
}

tPosD lastD(tListD L)
{
    tPosD p;
    for (p = L; p->next != LNULL; p = p->next);
    return p;
}

tPosD nextD(tPosD p, tListD L)
{
    return p->next;
}

tPosD previousD(tPosD p, tListD L)
{
    tPosD q;

    if (p == L) {
        return LNULL;
    } else {
        for (q = L; q->next != p; q = q->next);
        return q;
    }
}

int countListD(tListD L)
{
	int count = 0;
	tPosD p = firstD(L);

	for (int i = 0; p != NULL; p = p->next)
	{
		count++;
	}

	return count;
}

