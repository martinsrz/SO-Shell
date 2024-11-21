#include "memory.h"
#include "memoryList.h"
#include <stdio.h>

void Recursiva (int n)
{
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

    if (n>0) Recursiva(n-1);
}

void MemoryFuncs()
{
    printf("Funciones programa\t%p,\t\t%p,\t\t%p\n", (void *)MemoryFuncs, (void *)Recursiva, (void *)Do_pmap);
    printf("Funciones libreria\t%p,\t\t%p,\t\t%p\n", (void *)printf, (void *)malloc, (void *)exit);
}

void Do_pmap () /*sin argumentos*/
{
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4] = {"pmap", elpid, NULL};

    sprintf (elpid, "%d", (int) getpid());

    if ((pid = fork()) == -1)
    {
        perror ("Imposible crear proceso");
        return;
    }

    if (pid == 0)
    {
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0] = "procstat"; argv[1] = "vm"; argv[2] = elpid; argv[3] = NULL;
        if (execvp(argv[0], argv) == -1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0] = "procmap", argv[1] = elpid; argv[2] = NULL;
        if (execvp(argv[0], argv) == -1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        argv[0] = "vmmap"; argv[1] = "-interleave"; argv[2] = elpid; argv[3] = NULL;
        if (execvp(argv[0], argv) == -1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");
        exit(1);
    }

    waitpid (pid, NULL, 0);
 }