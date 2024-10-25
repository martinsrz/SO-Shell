#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "list.h"

void shellLoop(bool exit);
void printPrompt();
void readCommand(command *command, tList *commandList);
void processCommand(command *cmnd, tList *commandList, tList *openFiles, bool *exit);
void commands(tList *commandList, tList *openFiles, bool *exit, char *param1, char *param2);
void cmdAuthors(char *param2);
void cmdPid();
void cmdPpid();
void cmdCd();
void cmdDate(char *param2);
void cmdHistoric(char *param2, tList historic, tList *openFiles, bool *exit);
void cmdOpen(char *param2, tList *openFiles);
void cmdClose(char *param2, tList *openFiles);
void cmdDup();
void cmdInfosys();
void cmdMakefile(char *param2);
void cmdMakedir(char *param2);
void cmdListfile(char *param2);
void cmdCwd();
void cmdListdir(char *param2);
void cmdReclist();
void cmdRevlist();
void cmdErase(char *directory);
void cmdDelrec(char *directory);
void cmdHelp(char *param2);
void initOpenFiles(tList *openFiles);
void printOpenFiles(tList openFiles);

int main()
{
    bool exit = false;

    shellLoop(exit);

    return 0;
}

void limpiarCadena(char *str)
{
    char *end;

    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r' || *end == '\t'))
    {
        *end = '\0';
        end--;
    }
}

int separarArgumentos(char *cadena, char *trozos[])
{
    char *token = strtok(cadena, " \n\t");
    if (!token) return 0;

    trozos[0] = token;      // Primer string

    token = strtok(NULL, "");
    if (token != NULL)              // Resto de la cadena
    {
        limpiarCadena(token);
        trozos[1] = token;
    } else
    {
        trozos[1] = NULL;
    }

    return 2;
}

int trocearCadena(char *cadena, char *trozos[])
{
    int i=1;

    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

char LetraTF (mode_t m)
{
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
    }
}

char *convierteModo (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");

    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

void shellLoop(bool exit)
{
	command entry;
    tList history, openFiles;

    createEmptyList(&history);
    createEmptyList(&openFiles);
    initOpenFiles(&openFiles);

    while (!exit)
    {
        printPrompt();
        readCommand(&entry, &history);
        processCommand(&entry, &history, &openFiles, &exit);
    }

    deleteList(&history);
    deleteList(&openFiles);
}

void printPrompt()
{
    printf("-> ");
}

void readCommand(command *command, tList *commandList)
{
    tItem item;
    char *tr[COMMAND_LEN];

    fgets(*command, COMMAND_LEN, stdin);

    strcpy(item.command, *command);
    insertItem(item, LNULL, commandList);

    trocearCadena(item.command, tr);

    if (strcmp(tr[0], "command") == 0) {
        deleteAtPosition(last(*commandList), commandList);
    }
}

void processCommand(command *cmnd, tList *commandList, tList *openFiles, bool *exit)
{
    command aux;
    char *tr[COMMAND_LEN];
    strcpy(aux, *cmnd);
    int parameters = separarArgumentos(*cmnd, tr);

    if (parameters == 0) return;

    commands(commandList, openFiles, exit, tr[0], tr[1]);
}

void commands(tList *commandList, tList *openFiles, bool *exit, char *param1, char *param2)
{
    if (strcmp(param1, "authors") == 0)
    {
        cmdAuthors(param2);
    }
    else if (strcmp(param1, "pid") == 0)
    {
        cmdPid();
    }
    else if (strcmp(param1, "ppid") == 0)
    {
        cmdPpid();
    }
    else if (strcmp(param1, "cd") == 0)
    {
        cmdCd(param2);
    }
    else if (strcmp(param1, "date") == 0)
    {
        cmdDate(param2);
    }
    else if (strcmp(param1, "historic") == 0)
    {
        cmdHistoric(param2, *commandList, openFiles, exit); //hacer
    }
    else if (strcmp(param1, "open") == 0)
    {
        cmdOpen(param2, openFiles);
    }
    else if (strcmp(param1, "close") == 0)
    {
        cmdClose(param2, openFiles);
    }
    else if (strcmp(param1, "dup") == 0)
    {
        cmdDup(param2, openFiles);
    }
    else if (strcmp(param1, "infosys") == 0)
    {
        cmdInfosys();
    }
    else if (strcmp(param1, "makefile") == 0)
    {
        cmdMakefile(param2);
    }
    else if (strcmp(param1, "makedir") == 0)
    {
        cmdMakedir(param2);
    }
    else if (strcmp(param1, "listfile") == 0)
    {
        cmdListfile(param2);
    }
    else if (strcmp(param1, "cwd") == 0)
    {
        cmdCwd();
    }
    else if (strcmp(param1, "listdir") == 0)
    {
        cmdListdir(param2);
    }
    else if (strcmp(param1, "erase") == 0)
    {
        cmdErase(param2);
    }
    else if (strcmp(param1, "delrec") == 0)
    {
        cmdDelrec(param2);
    }
    else if (strcmp(param1, "help") == 0)
    {
        cmdHelp(param2);
    }
    else if ((strcmp(param1, "exit") == 0) || (strcmp(param1, "quit") == 0) || (strcmp(param1, "bye") == 0))
    {
        *exit = true;
    }
    else printf("Comando no reconocido\n");
}

void cmdAuthors(char *param2)
{
    if (param2 == NULL)
    {
        printf("Martin Tubio: m.tubio@udc.es\nDiego Candal: d.candal@udc.es\n");
    }
    else if (strcmp(param2, "-l") == 0)
    {
        printf("m.tubio@udc.es\nd.candal@udc.es\n");
    }
    else if (strcmp(param2, "-n") == 0)
    {
        printf("Martin Tubio\nDiego Candal\n");
    }
    else printf("Comando no reconocido\n");
}

void cmdPid()
{
    printf("Pid de shell: %d\n", getpid());
}

void cmdPpid()
{
    printf("Pid del padre de shell: %d\n", getppid());
}

void cmdCd(char *param2)
{
    if (param2 == NULL)
    {
        char dir[256];
        getcwd(dir, sizeof(dir));
        printf("%s\n", dir);
    }
    else
    {
        if (chdir(param2) == -1)
        {
            perror("Imposible cambiar directorio");
        }
    }
}

void cmdDate(char *param2)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    if (param2 == NULL)
    {
        printf("%02d/%02d/%04d\n%02d:%02d:%02d\n", tm->tm_mday, 1 + tm->tm_mon, 1900 + tm->tm_year, tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    else if (strcmp(param2, "-t") == 0)
    {
        printf("%02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
    else if (strcmp(param2, "-d") == 0)
    {
        printf("%02d/%02d/%04d\n", tm->tm_mday, 1 + tm->tm_mon, 1900 + tm->tm_year);
    }
    else printf("Comando no reconocido\n");
}

void cmdHistoric(char *param2, tList historic, tList *openFiles, bool *exit)
{
    if (param2 == NULL)
    {
        int i = 0;
        tPos p = first(historic);

        while (p != NULL)
        {
            printf("%d -> %s", i, p->data.command);

            p = p->next;
            i++;
        }
    }
    else if (isdigit(param2[0]))
    {
        command aux;
        int n = atoi(param2);
        char *tr[COMMAND_LEN];

        if (isEmptyList(historic))
        {
            printf("Historial vacio\n");
            return;
        }

        tPos p = findPosition(n, historic);

        if (p == NULL)
        {
            printf("El elemento %d no existe\n", n);
        }
        else
        {
            strcpy(aux, p->data.command);
            printf("Ejecutando hist (%d): %s\n", n, aux);

            trocearCadena(aux, tr);
            commands(&historic, openFiles, exit, tr[0], tr[1]);
        }

    }
    else if (param2[0] == '-' && isdigit(param2[1]))
    {
        param2[0] = '0';
        int n = atoi(param2);
        tPos p = last(historic);

        int counter = countList(historic);

        for (int j = 0; p != NULL && j < n - 1; j++)
        {
            p = previous(p, historic);
        }

        for (int i = counter - n - 1; p != NULL && i <= counter; i++)
        {
            printf("%d -> %s", i + 1, p->data.command);
            p = p->next;
        }
    }
    else printf("Comando no reconocido\n");
}

void cmdOpen(char *param2, tList *openFiles)
{
    int df;
    int mode = 0;
    tItem file;
    char *tr[COMMAND_LEN];

    trocearCadena(param2, tr);

    if (tr[0] == NULL)
    {
        printOpenFiles(*openFiles);
        return;
    }

    if (tr[1] == NULL)
    {
        mode |= O_RDONLY;
    }
    else
    {
        if (strcmp(tr[1], "cr") == 0) mode |= O_CREAT;
        else if (strcmp(tr[1], "ex") == 0) mode |= O_EXCL;
        else if (strcmp(tr[1], "ro") == 0) mode |= O_RDONLY;
        else if (strcmp(tr[1], "wo") == 0) mode |= O_WRONLY;
        else if (strcmp(tr[1], "rw") == 0) mode |= O_RDWR;
        else if (strcmp(tr[1], "ap") == 0) mode |= O_APPEND;
        else if (strcmp(tr[1], "tr") == 0) mode |= O_TRUNC;
        else {
            printf("Modo %s no reconocido\n", tr[1]);
            return;
        }
    }

    if ((df = open(param2, mode, 0777)) == -1 || countList(*openFiles) == 10)
    {
        perror("Imposible abrir fichero");
    }
    else
    {
        file.fileDescriptor = df;
        strcpy(file.command, param2);
        file.mode = mode;

        insertItem(file, LNULL, openFiles);

        printf("Añadida entrada %d a la tabla de ficheros abiertos\n", df);
    }
}

void cmdClose(char *param2, tList *openFiles)
{
    int df;
    tPos p;

    if (param2 == NULL || (df = atoi(param2)) < 0)
    {
        printOpenFiles(*openFiles);
        return;
    }

    p = findDescriptor(df, *openFiles);

    if (p == LNULL)
    {
        perror("Imposible cerrar descriptor");
        return;
    }

    if (close(p->data.fileDescriptor) == -1)
    {
        perror("Imposible cerrar descriptor");
    }
    else
    {
        deleteAtPosition(p, openFiles);
    }
}

void cmdDup(char *param2, tList *openFiles)
{
    int df;
    tPos p;
    tItem duplicado;
    char aux[64];

    if (param2 == NULL || (df = atoi(param2)) < 0)
    {
        printOpenFiles(*openFiles);
        return;
    }

    duplicado.fileDescriptor = dup(df);
    p = findDescriptor(df, *openFiles);

    if (p == LNULL)
    {
        perror("Imposible duplicar descriptor");
        return;
    }

    duplicado.mode = p->data.mode;

    strcpy(aux, p->data.command);
    sprintf (duplicado.command, "duplicado de %d (%s)", df, aux);

    if (duplicado.fileDescriptor != (-1))
    {
        insertItem(duplicado, LNULL, openFiles);
    }
    else
    {
        perror("Imposible duplicar descriptor");
    }
}

void cmdInfosys()
{
    struct utsname unameData;
    uname(&unameData);

    printf("%s (%s), OS: %s-%s-%s\n", unameData.nodename, unameData.machine, unameData.sysname, unameData.release, unameData.version);
}

void cmdMakefile(char *param2)
{
    if (param2 == NULL)
    {
        cmdCwd();
        return;
    }

    FILE *file;
    char path[256];
    char *filename = param2;
    getcwd(path, sizeof(path));
    strcat(path, "/");
    strcat(path, filename);

    if ((file = fopen(path, "w")) == NULL) {
        perror("Error al crear el archivo");
        return;
    }

    fclose(file);
}

void cmdMakedir(char *param2)
{
    if (param2 == NULL)
    {
        cmdCwd();
        return;
    }

    char path[256];
    char *foldername = param2;
    getcwd(path, sizeof(path));
    strcat(path, "/");
    strcat(path, foldername);

    if (mkdir(path, 0755) == -1)
    {
        perror("Error al crear la carpeta");
    }
}

void cmdListfile(char *param2)
{
    if (param2 == NULL)
    {
        cmdCwd();
        return;
    }

    struct stat fileStat;
    char *tr[COMMAND_LEN];
    char fecha[20], acc[20], path[256], fullPath[PATH_MAX];
    char *permisos;
    int arguments = trocearCadena(param2, tr);
    int lastArgument = 0;
    int cmdMode[3] = {0,0,0};

    if (arguments > 0)
    {
        if (strcmp(tr[lastArgument], "-long") == 0) cmdMode[0] = 1, lastArgument++;
        if (lastArgument < arguments && strcmp(tr[lastArgument], "-acc") == 0) cmdMode[1] = 1, lastArgument++;
        if (lastArgument < arguments && strcmp(tr[lastArgument], "-link") == 0) cmdMode[2] = 1, lastArgument++;
    }

    if (tr[lastArgument] == NULL)   // no hay filename despues de los parametros opcionales
    {
        cmdCwd();
        return;
    }

    getcwd(path, sizeof(path));
    strcat(path, "/");

    for (int i = lastArgument; i < arguments; i++)
    {
        char *filename = tr[i];
        snprintf(fullPath, sizeof(fullPath), "%s%s", path, filename);

        if (lstat(fullPath, &fileStat) == -1)
        {
            perror("****error al acceder al archivo");
        }
        else
        {
            if (cmdMode[0] == 1)
            {
                struct tm *timeinfo = localtime(&fileStat.st_mtime);
                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", timeinfo);
                permisos = convierteModo(fileStat.st_mode);
                timeinfo = localtime(&fileStat.st_mtime);
                struct passwd *pw = getpwuid(fileStat.st_uid);
                struct group *gr = getgrgid(fileStat.st_gid);

                printf("%s %3ld (%8lu)%s%s %s %8ld %s\n", fecha, fileStat.st_nlink, fileStat.st_ino, pw ? pw->pw_name : "???",
                gr ? gr->gr_name : "???", permisos, fileStat.st_size, filename);
            }
            else if (cmdMode[1] == 1)
            {
                struct tm *timeinfo = localtime(&fileStat.st_atime);
                strftime(acc, sizeof(acc), "%Y/%m/%d-%H:%M", timeinfo);

                printf("%8ld  %s %s\n", fileStat.st_size, acc, filename);
            }
            else if (cmdMode[2] == 1)
            {
                char filetype = LetraTF(fileStat.st_mode);

                if (filetype == 'l')
                {
                    char linkPath[PATH_MAX] = "";
                    ssize_t len = readlink(fullPath, linkPath, sizeof(linkPath)-1);

                    if (len == -1)
                    {
                        perror("Error al leer el enlace simbolico");
                    }

                    printf("%8ld %s", fileStat.st_size, filename);
                    printf(" -> %s\n", linkPath);
                }
                else
                {
                    printf("%8ld %s\n", fileStat.st_size, filename);
                }
            }
            else
            {
                printf("%8ld %s\n", fileStat.st_size, filename);
            }
        }
    }
}

void cmdCwd()
{
    char dir[256];
    getcwd(dir, sizeof(dir));
    printf("%s\n", dir);
}

void cmdListdir(char *param2)
{
    if (param2 == NULL)
    {
        cmdCwd();
        return;
    }

    struct stat fileStat;
    struct dirent *entry;
    DIR *dir;
    char *tr[COMMAND_LEN];
    char fecha[20], acc[20], path[256], fullPath[PATH_MAX];
    char *permisos;
    int arguments = trocearCadena(param2, tr);
    int lastArgument = 0;
    int cmdMode[4] = {0, 0, 0, 0};

    if (arguments > 0)
    {
        if (strcmp(tr[lastArgument], "-hid") == 0) cmdMode[0] = 1, lastArgument++;
        if (lastArgument < arguments && strcmp(tr[lastArgument], "-long") == 0) cmdMode[1] = 1, lastArgument++;
        if (lastArgument < arguments && strcmp(tr[lastArgument], "-acc") == 0) cmdMode[2] = 1, lastArgument++;
        if (lastArgument < arguments && strcmp(tr[lastArgument], "-link") == 0) cmdMode[3] = 1, lastArgument++;
    }

    if (tr[lastArgument] == NULL)
    {
        cmdCwd();
        return;
    }

    getcwd(path, sizeof(path));
    snprintf(path, sizeof(path), "%s", tr[lastArgument]);

    if ((dir = opendir(path)) == NULL)
    {
        perror("****error al abrir el directorio");
        return;
    }

    printf("************%s\n", path);

    while ((entry = readdir(dir)) != NULL)
    {
        if (!cmdMode[0] && entry->d_name[0] == '.')
        {
            // si encuentra un archivo oculto sin -hid no lo muestra por ello no hacemos nada
        }
        else
        {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

            if (lstat(fullPath, &fileStat) == -1)
            {
                perror("****error al acceder al archivo");
            }
            else
            {
                if (cmdMode[1] == 1)
                {
                    struct tm *timeinfo = localtime(&fileStat.st_mtime);
                    strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", timeinfo);
                    permisos = convierteModo(fileStat.st_mode);
                    struct passwd *pw = getpwuid(fileStat.st_uid);
                    struct group *gr = getgrgid(fileStat.st_gid);

                    printf("%s %3ld (%8lu) %s %s %s %8ld %s\n", fecha, fileStat.st_nlink, fileStat.st_ino,
                           pw ? pw->pw_name : "???", gr ? gr->gr_name : "???", permisos, fileStat.st_size, entry->d_name);
                }
                else if (cmdMode[2] == 1)
                {
                    struct tm *timeinfo = localtime(&fileStat.st_atime);
                    strftime(acc, sizeof(acc), "%Y/%m/%d-%H:%M", timeinfo);
                    printf("%8ld  %s %s\n", fileStat.st_size, acc, entry->d_name);
                }
                else if (cmdMode[3] == 1)
                {
                    char filetype = LetraTF(fileStat.st_mode);
                    if (filetype == 'l') {
                        char linkPath[PATH_MAX] = "";
                        ssize_t len = readlink(fullPath, linkPath, sizeof(linkPath) - 1);

                        if (len == -1)
                        {
                            perror("Error al leer el enlace simbolico");
                            continue;
                        }
                        linkPath[len] = '\0'; // Null-terminar la cadena

                        printf("%8ld %s -> %s\n", fileStat.st_size, entry->d_name, linkPath);
                    }
                    else
                    {
                        printf("%8ld %s\n", fileStat.st_size, entry->d_name);
                    }
                }
                else
                {
                    printf("%8ld %s\n", fileStat.st_size, entry->d_name);
                }
            }
        }
    }

    closedir(dir);
}

bool isEmptyDir(const char *directory)
{
    struct dirent *entry;
    int entry_count = 0;
    DIR *d = opendir(directory);
    while ((entry = readdir(d)) != NULL)
    {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) continue;
        entry_count ++;
    }
    closedir(d);
    return entry_count == 0;
}


void cmdErase(char *param2)
{
    if (param2 == NULL)
    {
        cmdCwd();
        return;
    }

    char *directory = param2;
    struct stat fileStat;

    if (stat(directory, &fileStat) == -1)
    {
        perror("No existe el archivo o directorio");
        return;
    }

    char filetype = LetraTF(fileStat.st_mode);

    if(filetype == 'd')
    {
        if(isEmptyDir(directory))
        {
            rmdir(directory);
        }
        else
            {
            perror("El directorio no esta vacio");
        }
    }
    else if(remove(directory) != 0)
    {
        perror("El directorio no es valido");
    }
}

void cmdDelrec(char *directory)
{
    if (directory == NULL)
    {
        cmdCwd();
        return;
    }
    if (strcmp(directory, ".") == 0 || strcmp(directory, "..") == 0)
    {
        perror("Error al eliminar el directorio");
        return;
    }

    struct stat fileStat;
    struct dirent *entry;
    DIR *dir;
    char aux[PATH_MAX];

    if (stat(directory, &fileStat) == -1)
    {
        perror("No existe el archivo o directorio");
        return;
    }

    char filetype = LetraTF(fileStat.st_mode);

    if (filetype == 'd')
    {
        dir = opendir(directory);
        if (dir == NULL)
        {
            perror("No se pudo abrir el directorio");
            return;
        }

        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                // no hacemos nada cuando los archivos sean . o ..
            }
            else
            {
                snprintf(aux, sizeof(aux), "%s/%s", directory, entry->d_name);

                cmdDelrec(aux);
            }
        }
        closedir(dir);

        if (rmdir(directory) != 0)
        {
            perror("Error al eliminar el directorio");
        }
    }
    else
    {
        if (remove(directory) != 0)
        {
            perror("Error al eliminar el archivo");
        }
    }
}

void cmdHelp(char *param2)
{
    if (param2 == NULL)
    {
        printf("'help [cmd|-lt|-T topic]' ayuda sobre comandos\n\t\t\t\tComandos disponibles:\nauthors pid ppid cd date historic open close dup infosys help quit exit bye\n");
    }
    else if (strcmp(param2, "authors") == 0)
    {
        printf("authors [-n|-l] Muestra los nombres y/o logins de los autores\n");
    }
    else if (strcmp(param2, "pid") == 0)
    {
        printf("pid\t\tMuestra el pid del shell\n");
    }
    else if (strcmp(param2, "ppid") == 0)
    {
        printf("pid\t\tMuestra el pid del padre del shell\n");
    }
    else if (strcmp(param2, "cd") == 0)
    {
        printf("cd [dir]\tCambia (o muestra) el directorio actual del shell\n");
    }
    else if (strcmp(param2, "date") == 0)
    {
        printf("date\t\tMuestra la fecha y hora actual\n");
    }
    else if (strcmp(param2, "historic") == 0)
    {
        printf("hist [N|-N]\tMuestra el historial de comandos\n\t\tN: repite el N comando\n\t\t-N: muestra los N ultimos comandos\n");
    }
    else if (strcmp(param2, "open") == 0)
    {
        printf("open file m1 m2...\n\t\tAbre el fichero file y lo anade a la lista de ficherosabiertos de la shell\n\t\tm1, m2..es el modo de apertura (or bit a bit de los siguientes).\n\t\tcr: O_CREAT\tap: O_APPEND\n\t\tex:O_EXCL\tro:O_RDONLY\n\t\trw: O_RDWR\two: O_WRONLY\n\t\ttr: O_TRUNC\n");
    }
    else if (strcmp(param2, "close") == 0)
    {
        printf("close df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
    }
    else if (strcmp(param2, "dup") == 0)
    {
        printf("dup df\t\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
    }
    else if (strcmp(param2, "infosys") == 0)
    {
        printf("infosys\t\tMuestra informacion de la maquina donde se ejecuta el shell\n");
    }
    else if (strcmp(param2, "help") == 0)
    {
        printf("help [cmd]\tMuestra ayuda sobre los comandos\n\t\tcmd: info sobre el comando cmd\n");
    }
    else if (strcmp(param2, "quit") == 0)
    {
        printf("quit\t\tFinaliza la ejecucion de la shell\n");
    }
    else if (strcmp(param2, "exit") == 0)
    {
        printf("exit\t\tFinaliza la ejecucion de la shell\n");
    }
    else if (strcmp(param2, "bye") == 0)
    {
        printf("bye\t\tFinaliza la ejecucion de la shell\n");
    }
    else printf("Comando %s no encontrado\n", param2);
}

void initOpenFiles(tList *openFiles)
{
    tItem d;
    int flags;

    d.fileDescriptor = STDIN_FILENO;
    strcpy(d.command, "entrada estandar");
    flags = fcntl(d.fileDescriptor, F_GETFL);
    d.mode = 0;
    d.mode = flags & O_ACCMODE;
    insertItem(d, LNULL, openFiles);

    d.fileDescriptor = STDOUT_FILENO;
    strcpy(d.command, "salida estandar");
    flags = fcntl(d.fileDescriptor, F_GETFL);
    d.mode = 0;
    d.mode = flags & O_ACCMODE;
    insertItem(d, LNULL, openFiles);

    d.fileDescriptor = STDERR_FILENO;
    strcpy(d.command, "error estandar");
    flags = fcntl(d.fileDescriptor, F_GETFL);
    d.mode = 0;
    d.mode = flags & O_ACCMODE;
    insertItem(d, LNULL, openFiles);
}

void printOpenFiles(tList openFiles)
{
    tPos p = first(openFiles);
    tItem d;
    off_t offset;

    for (int i = 0; i < 20; i++)
    {
        printf("descriptor: %d, offset: (", i);

        if (p != LNULL)
        {
            d = getItem(p, openFiles);

            if ((offset = lseek(d.fileDescriptor, 0, SEEK_CUR)) == -1)
            {
                printf("  )-> ");
            }
            else
            {
                printf("%ld)-> ", offset);
            }

            printf("%s ", d.command); // Mostrar el nombre del archivo

            if (d.mode & O_CREAT) printf("O_CREAT ");
            if (d.mode & O_EXCL) printf("O_EXCL ");
            if (d.mode & O_RDONLY) printf("O_RDONLY ");
            if (d.mode & O_WRONLY) printf("O_WRONLY ");
            if (d.mode & O_RDWR) printf("O_RDWR ");
            if (d.mode & O_APPEND) printf("O_APPEND ");
            if (d.mode & O_TRUNC) printf("O_TRUNC ");

            printf("\n");

            p = next(p, openFiles);
        }
        else
        {
            printf("  )-> no usado\n");
        }
    }
}