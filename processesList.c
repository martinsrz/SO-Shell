// Martín Tubio Suaŕez
// Diego Candal Varela

#include "processesList.h"

bool createNodeP(tPosP *p)
{
    *p = malloc(sizeof(struct tNodeP));

    return *p != LNULL;
}

void createEmptyListP(tListP *L)
{
    *L = LNULL;
}

bool insertItemP(tItemP d, tPosP p, tListP *L)
{
  	tPosP q, r;

 	if (!createNodeP(&q)) {
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

void updateItemP(tItemP d, tPosP p, tListP *L)
{
	p->data = d;
}

void deleteAtPositionP(tPosP p, tListP *L)
{
  	tPosP q;

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

void deleteListP(tListP *L)
{
    tPosP p;

	while (*L != LNULL)
	{
		p = *L;
		*L = p->next;
		free(p);
	}
}

bool isEmptyListP(tListP L)
{
  return (L == LNULL);
}

tPosP findItemP(pid_t pid, tListP L)
{
    tPosP p;
    for (p = L; (p != LNULL) && (p->data.pid != pid); p = p->next);
	if (p != NULL &&
		(p->data.pid == pid))
		return p;
	else
		return NULL;
}

tPosP findPositionP(int n, tListP L)
{
	tPosP pos = L;
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

tItemP getItemP(tPosP p, tListP L)
{
    return p->data;
}

tPosP firstP(tListP L)
{
    return L;
}

tPosP lastP(tListP L)
{
    tPosP p;
    for (p = L; p->next != LNULL; p = p->next);
    return p;
}

tPosP nextP(tPosP p, tListP L)
{
    return p->next;
}

tPosP previousP(tPosP p, tListP L)
{
    tPosP q;

    if (p == L) {
        return LNULL;
    } else {
        for (q = L; q->next != p; q = q->next);
        return q;
    }
}

int countListP(tListP L)
{
	int count = 0;
	tPosP p = firstP(L);

	for (int i = 0; p != NULL; p = p->next)
	{
		count++;
	}

	return count;
}

