#include "processes.h"
#include "memoryList.h"
#include "processesList.h"
#include "directoriesList.h"
#include "list.h"
#include <stdio.h>

extern char **environ;

/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del nÃºmero y viceversa */
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
    for (i=0; sigstrnum[i].nombre != NULL; i++)
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

bool isUppercase(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isupper(str[i])) {
            return false;
        }
    }
    return true;
}

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

void showSearchDirectories(tListD processesList)
{
    if (isEmptyListD(processesList)) return;

    tPosD pos;
    tItemD item;

    for (pos = firstD(processesList); pos != NULL; pos = nextD(pos, processesList))
    {
        item = getItemD(pos, processesList);
        printf("%s\n", item.directory);
    }
}

void searchAddDir(char *dir, tListD *directoriesList)
{
    tItemD item;

    item.directory = strdup(dir);

    if (findItemD(dir, *directoriesList) == NULL)
    {
        insertItemD(item, NULL, directoriesList);
    }
    else
    {
        printf("El directorio (%s) ya existe en la lista de búsqueda\n", item.directory);
        free(item.directory); // Liberamos si no se inserta
    }
}

void searchDelDir(char *dir, tListD *directoriesList)
{
    tPosD pos;

    if ((pos = findItemD(dir, *directoriesList)) != NULL)
    {
        deleteAtPositionD(pos, directoriesList);
    }
    else
    {
        printf("El directorio (%s) no existe en la lista de búsqueda\n", dir);
    }
}

void searchPath(tListD *directoriesList)
{
    int counter = 0;
    tItemD item;
    char *pathEnv = getenv("PATH");

    if (pathEnv == NULL) {
        perror("No se pudo acceder a la variable PATH");
        return;
    }

    char *pathCopy = strdup(pathEnv);   // hacemos una copia para proteger el valor original de PATH
    if (pathCopy == NULL) {
        perror("Error al duplicar PATH");
        return;
    }

    char *token = strtok(pathCopy, ":");
    while (token != NULL)
    {
        item.directory = strdup(token);

        if (findItemD(token, *directoriesList) == NULL)
        {
            insertItemD(item, NULL, directoriesList);
            counter++;
        }
        else
        {
            free(item.directory); // Liberamos si no se inserta
        }

        token = strtok(NULL, ":");
    }

    printf("Importados %d directorios en la ruta de búsqueda\n", counter);

    free(pathCopy);
}

char **createEnvironment(char *vars[], int nVars, tListM *envList)
{
    tItemM var;
    char *value;
    char **newEnv = malloc((nVars + 1) * sizeof(char *));
    if (newEnv == NULL) return NULL;

    int newEnvIdx = 0;

    for (int i = 0; i < nVars; i++)
    {
        value = getenv(vars[i]);
        if (value != NULL)
        {
            size_t len = strlen(vars[i]) + strlen(value) + 2;
            newEnv[newEnvIdx] = malloc(len * sizeof(char));
            if (newEnv[newEnvIdx] == NULL)
            {
                return NULL;
            }
            snprintf(newEnv[newEnvIdx], len, "%s=%s", vars[i], value);
            var.memoryAddress = newEnv[newEnvIdx];
            insertItemM(var, NULL, envList);
            newEnvIdx++;
        }
    }

    newEnv[newEnvIdx] = NULL;
    var.memoryAddress = newEnv;
    insertItemM(var, NULL, envList);
    return newEnv;
}

void separarEntrada(char *tr[], int nArgs, char *vars[], char *trozos[], int *varsCount, int *trozosCount)
{
    {
        int varsIndex = 0, trozosIndex = 0;
        bool foundExecutable = false;

        for (int i = 0; i < nArgs; i++) {
            if (!foundExecutable && isUppercase(tr[i]) && getenv(tr[i]) != NULL) {
                // Es una variable válida
                vars[varsIndex++] = tr[i];
            } else {
                // El resto pertenece al ejecutable y sus argumentos
                foundExecutable = true;
                trozos[trozosIndex++] = tr[i];
            }
        }

        // Actualizamos los contadores
        *varsCount = varsIndex;
        *trozosCount = trozosIndex;

        // Agregamos terminadores NULL por compatibilidad
        vars[varsIndex] = NULL;
        trozos[trozosIndex] = NULL;
    }
}

void exec(char *tr[], int nArgs, tListD directoriesList, tListM *envList)
{
    char **newEnv;
    char *vars[nArgs], *trozos[nArgs];
    int varsCount, trozosCount;

    separarEntrada(tr, nArgs, vars, trozos, &varsCount, &trozosCount);

    if (varsCount > 0)
        newEnv = createEnvironment(vars, varsCount, envList);
    else
        newEnv = NULL;

    if (Execpve(trozos, newEnv, NULL, directoriesList) == -1)
        perror("Imposible ejecutar");
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
    tItemD item;
    static char path[MAXNAME];
    struct stat st;
    char *p;

    if (isEmptyListD(directoriesList)) return NULL;

    tPosD d = firstD(directoriesList);

    if (s == NULL || d == NULL)
        return s;
    if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;        /*is an absolute pathname*/


    item = getItemD(d, directoriesList);
    p = item.directory;

    strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
    if (lstat(path,&st)!=-1)
        return path;

    d = nextD(d, directoriesList);

    while (d != NULL)
    {
        item = getItemD(d, directoriesList);
        p = item.directory;

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