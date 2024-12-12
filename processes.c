#include "processes.h"
#include "processesList.h"
#include "directoriesList.h"
#include "list.h"
#include <stdio.h>

extern char **environ;

char *userName(uid_t uid)
{
    struct passwd *d;

    if ((d = getpwuid(uid)) == NULL)
        return "??????";

    return d->pw_name;
}

void getUid()
{
    printf("Credencial real: %d, (%s)\n", getuid(), userName(getuid()));
    printf("Credencial efectiva: %d, (%s)\n", geteuid(), userName(geteuid()));
}

void uidSetId(char *id)
{
    if (id == NULL)
    {
        getUid();
        return;
    }

    uid_t uid = (uid_t) atoi(id);
    if (seteuid(uid) == -1)
        perror("Imposible cambiar credencial");
}

void uidSetUsername(char *username)
{
    struct passwd *d;

    if (username == NULL)
    {
        getUid();
        return;
    }

    if ((d = getpwnam(username)) == NULL)
    {
        printf("Usuario no existente %s\n", username);
        return;
    }

    if (seteuid(d->pw_uid) == -1)
        perror("Imposible cambiar credencial");
}

void environArg3(char *arg3[])
{
    for(int i = 0; arg3[i] != NULL; i++)
    {
        printf("%p->%s[%d]=(%p) %s\n", arg3[i], "main arg3", i, &arg3[i], arg3[i]);
    }
}

void environEnv()
{
    for (int i = 0; environ[i] != NULL; i++)
    {
        printf("%p->%s[%d]=(%p) %s\n", environ[i], "environ", i, &environ[i], environ[i]);
    }
}

void environAddr(char *arg3[])
{
    printf("environ: %p (almacenado en %p)\n", environ, &environ);
    printf("main arg3: %p (almacenado en %p)\n", arg3, &arg3);
}

void Cmd_fork(tListP *processesList)
{
    pid_t pid;

    if ((pid=fork())==0)
    {
        deleteListP(processesList);
        printf ("ejecutando proceso %d\n", getpid());
    }
    else if (pid!=-1)
        waitpid (pid,NULL,0);
}

int BuscarVariable(char *var, char *e[])  /*busca una variable en el entorno que se le pasa como parÃ¡metro*/
{                                           /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
    int pos=0;
    char aux[MAXVAR];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
    {
        if (!strncmp(e[pos],aux,strlen(aux)))
        {
            return (pos);
        }
        else pos++;
    }
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}

char *Ejecutable(char *s, tListD directoriesList)
{
    static char path[MAXNAME];
    struct stat st;
    char *p;

    tPosD d = firstD(directoriesList);

    if (s == NULL || (p = d->data.directory) == NULL)
        return s;
    if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;        /*is an absolute pathname*/

    strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
    if (lstat(path,&st)!=-1)
        return path;

    d = nextD(d, directoriesList);

    while ((p = d->data.directory) != NULL)
    {
        strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
        if (lstat(path,&st)!=-1)
            return path;
        d = nextD(d, directoriesList);
    }
    return s;
}

int Execpve(char *tr[], char **NewEnv, int *pprio, tListD directoriesList)
{
    char *p;               /*NewEnv contains the address of the new environment*/
    /*pprio the address of the new priority*/
    /*NULL indicates no change in environment and/or priority*/
    if (tr[0] == NULL || (p = Ejecutable(tr[0], directoriesList)) == NULL){
        errno=EFAULT;
        return-1;
    }
    if (pprio !=NULL  && setpriority(PRIO_PROCESS,getpid(),*pprio)==-1 && errno){
        printf ("Imposible cambiar prioridad: %s\n",strerror(errno));
        return -1;
    }

    if (NewEnv == NULL)
        return execv(p,tr);
    else
        return execve (p, tr, NewEnv);
}