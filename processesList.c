// Martín Tubio Suaŕez
// Diego Candal Varela

#include "processesList.h"
#include "processes.h"

static struct SEN sigstrnum[] = {
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL},
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2},
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP},
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH},
	{"IO", SIGIO},
	{"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
	 {NULL,-1},
	};    /*fin array sigstrnum */

int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/
{
	int i;
	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (!strcmp(sen, sigstrnum[i].nombre))
			return sigstrnum[i].senal;
	return -1;
}

char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/
{			/* para sitios donde no hay sig2str*/
	int i;
	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (sen==sigstrnum[i].senal)
			return sigstrnum[i].nombre;
	return ("SIGUNKNOWN");
}

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

void refreshItemP(tPosP p, tListP *L)
{
	int status;
	tItemP item = getItemP(p, *L);
	pid_t wpid = waitpid(item.pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

	item.priority = getpriority(PRIO_PROCESS, item.pid);

	if (wpid == 0)
		return;

	if (wpid == item.pid)
	{
		int signalNum = WTERMSIG(status);  // Solo válido si WIFSIGNALED es verdadero

		if (WIFEXITED(status)) {
			sprintf(item.status, "TERMINADO");
			sprintf(item.signal, "000"); // Sin señal relevante
		} else if (WIFSIGNALED(status)) {
			strcpy(item.status, "SENALADO");
			if (strcmp(NombreSenal(signalNum), "SIGUNKNOWN") == 0) {
				sprintf(item.signal, "%03d", signalNum);
			} else {
				strcpy(item.signal, NombreSenal(signalNum));
			}
		} else if (WIFSTOPPED(status)) {
			strcpy(item.status, "PARADO");
			sprintf(item.signal, "%03d", WSTOPSIG(status)); // Señal que lo detuvo
		} else if (WIFCONTINUED(status)) {
			strcpy(item.status, "ACTIVO");
			strcpy(item.signal, "000");
		}
		updateItemP(item, p, L);
	}

	updateItemP(item, p, L);
}