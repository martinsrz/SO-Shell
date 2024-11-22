#include "memory.h"
#include "memoryList.h"
#include "list.h"
#include <stdio.h>

int *vg1, *vg2, *vg3, *vg4, *vg5, *vg6;

void *cadtop(char *str)
{
    unsigned long long int dir = strtoull(str, NULL, 16);
    void *p = (void *)dir;
    return p;
}

void *MapearFichero(char *fichero, int protection, tListM *memoryList, tList *openFiles)
{
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;
    tItem file;
    tItemM item;

    if (protection & PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
        return NULL;
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED)
        return NULL;

    /* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    item.memoryAddress = p;
    item.size = s.st_size;
    item.time = tm;
    strcpy(item.mode, "mmap");
    item.key = -1;
    item.fd = df;
    strcpy(item.name, fichero);
    insertItemM(item, LNULL, memoryList);

    /* Guardar en la lista de descriptores usados df, fichero*/

    file.fileDescriptor = df;
    strcpy(file.command, fichero);
    file.mode = modo;
    file.dup = false;
    file.mmap = true;
    insertItem(file, LNULL, openFiles);

    return p;
}

void *ObtenerMemoriaShmget (key_t clave, size_t tam, tListM *memoryList)
{
    void *p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    tItemM item;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL; /*cuando no es crear pasamos de tamano 0*/
    if (clave==IPC_PRIVATE)  /*no nos vale*/
    {
        errno=EINVAL; return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL,0)) == (void*) -1)
    {
        aux = errno;
        if (tam)
            shmctl(id,IPC_RMID, NULL);
        errno = aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT, &s); /* si no es crear, necesitamos el tamano, que es s.shm_segsz*/

    /* Guardar en la lista   InsertarNodoShared (&L, p, s.shm_segsz, clave); */
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    item.memoryAddress = p;
    item.size = s.shm_segsz;
    item.time = tm;
    strcpy(item.mode, "shared");
    item.key = clave;
    item.fd = -1;
    strcpy(item.name, "");
    insertItemM(item, LNULL, memoryList);

    return (p);
}

ssize_t LeerFichero (char *f, void *p, size_t cont)
{
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

ssize_t EscribirFichero (char *f, void *p, size_t cont)
{
    ssize_t  n;
    int df, aux, flags = O_CREAT | O_EXCL | O_WRONLY;

    if ((df=open(f,flags,0777))==-1)
        return -1;

    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }

    close (df);
    return n;
}

ssize_t LeerDf(int fd, void *p, size_t cont)
{
    ssize_t n;

    // Si el descriptor no es válido
    if (fd < 0) {
        errno = EBADF;  // Error de descriptor de archivo inválido
        return -1;
    }

    // Si cont es -1, obtenemos el tamaño disponible en el archivo (opcional)
    if (cont == (size_t)-1) {
        off_t current_offset = lseek(fd, 0, SEEK_CUR);
        if (current_offset == (off_t)-1) {
            return -1;  // Error al obtener el offset actual
        }

        off_t end_offset = lseek(fd, 0, SEEK_END);
        if (end_offset == (off_t)-1) {
            return -1;  // Error al mover el offset al final
        }

        cont = end_offset - current_offset;

        // Restauramos el offset original
        if (lseek(fd, current_offset, SEEK_SET) == (off_t)-1) {
            return -1;  // Error al restaurar el offset original
        }
    }

    // Leer cont bytes del descriptor y escribir en *p
    n = read(fd, p, cont);
    if (n == -1) {
        return -1;  // Devolvemos el error si la lectura falla
    }

    return n;  // Retornamos la cantidad de bytes leídos
}

void do_AllocateMalloc(size_t size, tListM *memoryList)
{
    tItemM item;
    void *ptr = malloc(size);

    if (ptr == NULL)
    {
        perror("Error al asignar memoria con malloc");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    item.memoryAddress = ptr;
    item.size = size;
    item.time = tm;
    strcpy(item.mode, "malloc");
    item.key = -1;
    item.fd = -1;
    strcpy(item.name, "");

    printf("Asignados %zu bytes en %p\n", size, ptr);
    insertItemM(item, LNULL, memoryList);
}

void do_AllocateMmap(char *file, char *perms, tListM *memoryList, tList *openFiles)
{
    char *perm;
    void *p;
    int protection=0;

    if ((perm = perms) != NULL && strlen(perm) < 4)
    {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }

    if ((p = MapearFichero(file, protection, memoryList, openFiles)) == NULL)
        perror ("Imposible mapear fichero");
    else
        printf ("fichero %s mapeado en %p\n", file, p);
}

void do_AllocateCreateshared (char *clv, char *n, tListM *memoryList)
{
    key_t cl;
    size_t tam;
    void *p;

    cl = (key_t)  strtoul(clv, NULL,10);
    tam = (size_t) strtoul(n, NULL,10);
    if (tam == 0) {
        printf ("No se asignan bloques de 0 bytes\n");
        return;
    }
    if ((p = ObtenerMemoriaShmget(cl, tam, memoryList)) != NULL)
        printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
    else
        printf ("Imposible asignar memoria compartida clave %lu: %s\n", (unsigned long) cl, strerror(errno));
}

void do_AllocateShared(char *clv, tListM *memoryList)
{
    key_t cl;
    size_t tam;
    void *p;

    cl = (key_t)  strtoul(clv, NULL,10);

    if ((p=ObtenerMemoriaShmget(cl, 0, memoryList)) != NULL)
        printf ("Asignada memoria compartida de clave %lu en %p\n",(unsigned long) cl, p);
    else
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void do_DeallocateMalloc(size_t size, tListM *memoryList)
{
    if (!isEmptyListM(*memoryList))
    {
        tPosM pos;
        tItemM item;

        pos = firstM(*memoryList);

        while (pos != NULL)
        {
            item = getItemM(pos, *memoryList);

            if (item.size == size && strcmp(item.mode, "malloc") == 0)
            {
                free(item.memoryAddress);

                deleteAtPositionM(pos, memoryList);

                return;
            }

            pos = nextM(pos, *memoryList);
        }

        printf("No hay bloque de ese tamano asignado con malloc\n");
    }
    else printf("No hay bloque de ese tamano asignado con malloc\n");
}

void do_DeallocateMmap(char *file, tListM *memoryList, tList *openFiles)
{
    if (!isEmptyListM(*memoryList))
    {
        tPosM posM;
        tPos pos;
        tItemM itemM;

        posM = firstM(*memoryList);

        while (posM != LNULL)
        {
            itemM = getItemM(posM, *memoryList);
            pos = findItem(itemM.name, *openFiles);

            if (pos != LNULL && strcmp(itemM.name, file) == 0 && strcmp(itemM.mode, "mmap") == 0)
            {
                if (close(itemM.fd) != -1)
                {
                    if (munmap(itemM.memoryAddress, itemM.size) == -1)
                    {
                        perror("Error al desmapear el archivo");
                        return;
                    }

                    deleteAtPosition(pos, openFiles);
                    deleteAtPositionM(posM, memoryList);

                    return;
                }
            }

            posM = nextM(posM, *memoryList);
        }
        printf("Fichero %s no mapeado\n", file);
    }
    else printf("Fichero %s no mapeado\n", file);
}

void do_DeallocateShared(char *clv, tListM *memoryList)
{
    if (!isEmptyListM(*memoryList))
    {
        key_t cl;
        tPosM pos;
        tItemM item;

        pos = firstM(*memoryList);
        cl = (key_t)  strtoul(clv, NULL,10);

        while (pos != NULL)
        {
            item = getItemM(pos, *memoryList);

            if (item.key == cl && strcmp(item.mode, "shared") == 0)
            {
                if (shmdt(item.memoryAddress) == -1)
                {
                    perror("Error al desconectar memoria compartida");
                    return;
                }

                deleteAtPositionM(pos, memoryList);

                return;
            }

            pos = nextM(pos, *memoryList);
        }

        printf("No hay bloque de esa clave mapeado en el proceso\n");
    }
    else printf("No hay bloque de esa clave mapeado en el proceso\n");
}

void do_DeallocateDelkey (char *key)
{
    key_t clave;
    int id;

    if (key == NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

void do_Deallocate(char *address, tListM *memoryList, tList *openFiles)
{
    void *adr;
    adr = cadtop(address);

    if (!isEmptyListM(*memoryList))
    {
        tPosM pos;
        tItemM item;

        pos = firstM(*memoryList);

        while (pos != NULL)
        {
            item = getItemM(pos, *memoryList);

            if (item.memoryAddress == adr)
            {
                if (strcmp (item.mode, "malloc") == 0)
                {
                    free(item.memoryAddress);
                    deleteAtPositionM(pos, memoryList);
                }
                else if (strcmp (item.mode, "mmap") == 0)
                {
                    do_DeallocateMmap(item.name, memoryList, openFiles);
                }
                else if (strcmp (item.mode, "shared") == 0)
                {
                    char str[10];
                    snprintf(str, 10, "%d", item.key);
                    do_DeallocateShared(str, memoryList);
                }
                else
                {
                    printf("Direccion %p no asignada con malloc, shared o mmap\n", adr);
                }

                return;
            }

            pos = nextM(pos, *memoryList);
        }

        printf("Direccion %p no asignada con malloc, shared o mmap\n", adr);
    }
    else printf("Direccion %p no asignada con malloc, shared o mmap\n", adr);
}

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

void MemoryVars()
{
    // variables locales
    int *i = malloc(sizeof(int));
    int *j = malloc(sizeof(int));
    int *k = malloc(sizeof(int));
    // variables estaticas
    int vl1 = 1, vl2 = 2, vl3 = 3;
    // variables estaticas no inicializadas
    int vl4, vl5, vl6;
    // variables globales
    vg1 = malloc(sizeof(int));
    vg2 = malloc(sizeof(int));
    vg3 = malloc(sizeof(int));

    printf("Variables locales\t%p,\t\t%p,\t\t%p\n", &i, &j, &k);
    printf("Variables globales\t%p,\t\t%p,\t\t%p\n", &vg1, &vg2, &vg3);
    printf("Var (N.I.) globales\t%p,\t\t%p,\t\t%p\n", &vg4, &vg5, &vg6);
    printf("Variables estaticas\t%p,\t\t%p,\t\t%p\n", &vl1, &vl2, &vl3);
    printf("Var (N.I.) estaticas\t%p,\t\t%p,\t\t%p\n", &vl4, &vl5, &vl6);

    free(i);
    free(j);
    free(k);
    free(vg1);
    free(vg2);
    free(vg3);
}

void MemoryBlocks(char *type, tListM memoryList)    // si type es "" imprime todos
{
    tPosM pos;
    int pid = getpid();

    printf("******Lista de bloques asignados %s para el proceso %d\n", type, pid);

    if (!isEmptyListM(memoryList))
    {
        char fecha[20];
        pos = firstM(memoryList);

        while (pos != LNULL)
        {
            tItemM item = getItemM(pos, memoryList);
            strftime(fecha, sizeof(fecha), "%b %d %H:%M", item.time);

            if (strcmp(type, item.mode) == 0)
            {
                if (strcmp(item.mode, "mmap") == 0)
                {
                    printf("\t%p %16zu %s %s  (descriptor %d)\n", item.memoryAddress, item.size, fecha, item.name, item.fd);
                }
                else if (strcmp(item.mode, "shared") == 0)
                {
                    printf("\t%p %16zu %s %s (key %d)\n", item.memoryAddress, item.size, fecha, item.mode, item.key);
                }
                else
                {
                    printf("\t%p %16zu %s %s\n", item.memoryAddress, item.size, fecha, item.mode);
                }
            }
            else if (strcmp(type, "") == 0)
            {
                if (strcmp(item.mode, "mmap") == 0)
                {
                    printf("\t%p %16zu %s %s  (descriptor %d)\n", item.memoryAddress, item.size, fecha, item.name, item.fd);
                }
                else if (strcmp(item.mode, "shared") == 0)
                {
                    printf("\t%p %16zu %s %s (key %d)\n", item.memoryAddress, item.size, fecha, item.mode, item.key);
                }
                else
                {
                    printf("\t%p %16zu %s %s\n", item.memoryAddress, item.size, fecha, item.mode);
                }
            }

            pos = nextM(pos, memoryList);
        }
    }
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