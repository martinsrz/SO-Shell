#include "processes.h"
#include "memoryList.h"
#include "processesList.h"
#include "directoriesList.h"
#include "list.h"
#include <stdio.h>

extern char **environ;

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

void execFg(char *tr[], int nArgs, tListD directoriesList, tListM *envList)
{
    char **newEnv;
    char *vars[nArgs], *trozos[nArgs];
    int varsCount, trozosCount;

    separarEntrada(tr, nArgs, vars, trozos, &varsCount, &trozosCount);

    if (varsCount > 0)
        newEnv = createEnvironment(vars, varsCount, envList);
    else
        newEnv = NULL;

    if (Execforeground(trozos, newEnv, NULL, directoriesList) == -1)
        perror("Imposible ejecutar");
}

void execFgprio(char *tr[], int nArgs, tListD directoriesList, tListM *envList)
{
    char **newEnv;
    char *vars[nArgs-1], *trozos[nArgs-1];
    int varsCount, trozosCount;

    int prio = atoi(tr[0]);
    int *prioPtr = &prio;
    char **rest = &tr[1];

    separarEntrada(rest, nArgs-1, vars, trozos, &varsCount, &trozosCount);

    if (varsCount > 0)
        newEnv = createEnvironment(vars, varsCount, envList);
    else
        newEnv = NULL;

    if (Execforeground(trozos, newEnv, prioPtr, directoriesList) == -1)
        perror("Imposible ejecutar");
}

void execBack(char *tr[], int nArgs, tListD directoriesList, tListM *envList, tListP *processesList)
{
    char **newEnv;
    char *vars[nArgs], *trozos[nArgs];
    int varsCount, trozosCount;

    separarEntrada(tr, nArgs, vars, trozos, &varsCount, &trozosCount);

    if (varsCount > 0)
        newEnv = createEnvironment(vars, varsCount, envList);
    else
        newEnv = NULL;

    if (Execbackground(trozos, newEnv, NULL, directoriesList, processesList) == -1)
        perror("Imposible ejecutar");
}

void execBackpri(char *tr[], int nArgs, tListD directoriesList, tListM *envList, tListP *processesList)
{
    char **newEnv;
    char *vars[nArgs-1], *trozos[nArgs-1];
    int varsCount, trozosCount;

    int prio = atoi(tr[0]);
    int *prioPtr = &prio;
    char **rest = &tr[1];

    separarEntrada(rest, nArgs-1, vars, trozos, &varsCount, &trozosCount);

    if (varsCount > 0)
        newEnv = createEnvironment(vars, varsCount, envList);
    else
        newEnv = NULL;

    if (Execbackground(trozos, newEnv, prioPtr, directoriesList, processesList) == -1)
        perror("Imposible ejecutar");
}

void listjobs(tListP *processesList)
{
    tPosP p;
    tItemP item;

    for (p = firstP(*processesList); p != NULL; p = nextP(p, *processesList))
    {
        refreshItemP(p, processesList);
        item = getItemP(p, *processesList);

        printf("%6d   %s p=%d %s %s (%s) %s\n", item.pid, item.user, item.priority, item.time, item.status, item.signal, item.cmnd);
    }
}

void delterm(tListP *processesList)
{
    tPosP p;
    tItemP item;

    for (p = firstP(*processesList); p != NULL; p = nextP(p, *processesList))
    {
        item = getItemP(p, *processesList);

        if (strcmp(item.status, "TERMINADO") == 0) deleteAtPositionP(p, processesList);
    }

    listjobs(processesList);
}

void delsig(tListP *processesList)
{
    tPosP p;
    tItemP item;

    for (p = firstP(*processesList); p != NULL; p = nextP(p, *processesList))
    {
        item = getItemP(p, *processesList);

        if (strcmp(item.status, "TERMINADO") == 0 && strcmp(item.signal, "000") != 0) deleteAtPositionP(p, processesList);
    }

    listjobs(processesList);
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

int Execforeground(char *tr[], char **NewEnv, int *pprio, tListD directoriesList)
{
    char *p;

    if (tr[0] == NULL || (p = Ejecutable(tr[0], directoriesList)) == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (pprio != NULL && setpriority(PRIO_PROCESS, getpid(), *pprio) == -1) {
        printf("Imposible cambiar prioridad: %s\n", strerror(errno));
        return -1;
    }

    // Creamos un proceso hijo
    pid_t pid = fork();
    if (pid < 0) {
        return -1;  // Error al crear el proceso hijo
    }

    if (pid == 0) {
        // Estamos en el proceso hijo, ejecutamos el programa
        if (NewEnv == NULL) {
            if (execv(p, tr) == -1) {
                perror("Imposible ejecutar");
                exit(1);
            }
        } else {
            if (execve(p, tr, NewEnv) == -1) {
                perror("Imposible ejecutar");
                exit(1);
            }
        }
    } else {
        // Estamos en el proceso padre
        int status;
        // Esperamos que el proceso hijo termine
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
    }

    return 0;
}

int Execbackground(char *tr[], char **NewEnv, int *pprio, tListD directoriesList, tListP *processesList)
{
    char *p;

    // Si no se encuentra el ejecutable, retorna un error
    if (tr[0] == NULL || (p = Ejecutable(tr[0], directoriesList)) == NULL)
    {
        errno = EFAULT;
        return (-1);
    }

    // Cambiar la prioridad del proceso si es necesario
    if (pprio != NULL && setpriority(PRIO_PROCESS, getpid(), *pprio) == -1)
    {
        printf("Imposible cambiar prioridad: %s\n", strerror(errno));
        return (-1);
    }

    // Creamos un proceso hijo
    pid_t pid = fork();
    if (pid < 0) return (-1);  // Error al crear el proceso hijo

    if (pid == 0)
    {
        // Estamos en el proceso hijo, ejecutar el programa en segundo plano

        // Desasociar el proceso del terminal
        if (setsid() < 0) {
            perror("Imposible ejecutar");
            exit(1);
        }

        // Ejecutamos el programa
        if (NewEnv == NULL)
        {
            if (execv(p, tr) == -1) {
                perror("Imposible ejecutar");
                exit(1);
            }
        }
        else
        {
            if (execve(p, tr, NewEnv) == -1) {
                perror("Imposible ejecutar");
                exit(1);
            }
        }
    }
    else
    {
        struct passwd *usr;
        usr = getpwuid(getuid());
        time_t startedTime = time(NULL);
        struct tm *startedTimeInfo = localtime(&startedTime);
        char fecha[30];
        strftime(fecha, sizeof(fecha), "%Y/%m/%d %H:%M:%S", startedTimeInfo);

        tItemP item;

        item.pid = pid;
        strcpy(item.time, fecha);
        strcpy(item.status, "ACTIVO");
        item.priority = (pprio != NULL) ? *pprio : 0;
        strcpy(item.user, (usr != NULL) ? usr->pw_name : "unknown");
        strcpy(item.signal, "000");

        memset(item.cmnd, 0, sizeof(item.cmnd));

        for (int i = 0; tr[i] != NULL; i++)
        {
            strcat(item.cmnd, tr[i]);
            strcat(item.cmnd, " ");
        }

        insertItemP(item, NULL, processesList);
    }

    return 0;
}