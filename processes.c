#include "processes.h"
#include "memoryList.h"
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

void printArg3(char *var, char *arg3[])
{
    int i = BuscarVariable(var, arg3);

    if (i != -1)
    {
        printf("Con arg3 main %s(%p) @%p\n", arg3[i], arg3[i], &arg3[i]);
    }
}

void printEnv(char *var)
{
    int i = BuscarVariable(var, environ);

    if (i != -1)
    {
        printf("  Con environ %s(%p) @%p\n", environ[i], environ[i], &environ[i]);
    }
}

void printGetenv(char *var)
{
    char *env = getenv(var);

    if (env != NULL)
    {
        printf("   Con getenv %s(%p)\n", env, env);
    }
}

void changevar(char *tr0, char *tr1, char *tr2, char *arg3[], tListM *envList)
{
    if (strcmp(tr0, "-a") == 0)
    {
        if (CambiarVariable(tr1, tr2, arg3, envList) == -1)
            perror("Imposible cambiar variable de entorno");
        return;
    }
    if (strcmp(tr0, "-e") == 0)
    {
        if (CambiarVariable(tr1, tr2, environ, envList) == -1)
            perror("Imposible cambiar variable de entorno");
        return;
    }
    if (strcmp(tr0, "-p") == 0)
    {
        tItemM var;

        char *newvar;
        if ((newvar = (char *)malloc(strlen(tr1) + strlen(tr2) + 2)) == NULL) return;

        var.memoryAddress = newvar;
        insertItemM(var, NULL, envList);

        strcpy(newvar, tr1);
        strcat(newvar, "=");
        strcat(newvar, tr2);
        strcat(newvar, "\0");

        if (putenv(newvar) == -1)
            perror("Imposible cambiar variable de entorno");
    }
}

void subsvar(char *tr0, char *tr1, char *tr2, char *tr3, char *arg3[], tListM *envList)
{
    if (strcmp(tr0, "-a") == 0)
    {
        if (SustituirVariable(tr1, tr2, tr3, arg3, envList) == -1)
            perror("Imposible sustituir variable de entorno");
        return;
    }
    if (strcmp(tr0, "-e") == 0)
    {
        if (SustituirVariable(tr1, tr2, tr3, environ, envList) == -1)
            perror("Imposible sustituir variable de entorno");
    }
}

void environArg3(char *arg3[])
{
    for(int i = 0; arg3[i] != NULL; i++)
    {
        printf("%p->%s[%d]=(%p) %s\n", &arg3[i], "main arg3", i, arg3[i], arg3[i]);
    }
}

void environEnv()
{
    for (int i = 0; environ[i] != NULL; i++)
    {
        printf("%p->%s[%d]=(%p) %s\n", &environ[i], "environ", i, environ[i], environ[i]);
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

int CambiarVariable(char * var, char * valor, char *e[], tListM *envList) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
    int pos;
    char *aux;
    tItemM v;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;

    v.memoryAddress = aux;
    insertItemM(v, NULL, envList);

    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    e[pos]=aux;
    return (pos);
}

int SustituirVariable(char * var, char * var2, char * valor, char *e[], tListM *envList)
{
    int pos;
    char *aux;
    tItemM v;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;

    v.memoryAddress = aux;
    insertItemM(v, NULL, envList);

    strcpy(aux,var2);
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