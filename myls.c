#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>
#include <fcntl.h>


// blue to display the names of repertories in blue, etc...
#define BLUE    "\x1b[34m"
#define RED     "\x1b[31m"
#define BOLD 	"\x1b[1m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

int main (int argc, char **argv) {

int c;
char* all_value = NULL;
char* l_value= NULL;
char* recursive_value = NULL; 

int all_flag = 0;
int list_flag = 0;
int recursive_flag = 0;

struct option longopts [] = {
	{ "all", no_argument , NULL , 'a' },
	{ "recursive", no_argument , NULL , 'R' },
	{ 0, no_argument , NULL , 'l' },
	{ 0, 0, 0, 0 }
};

// structure to get informations about the files and repertories
struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

void ls(char *path){
    int fd, n, i;
    char buf[1024];
    struct linux_dirent *namelist;

    char file_name[255];
    char *file_path = path;
    char d_type;

    if(file_path == NULL){
        file_path = ".";
    }

    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do
    {
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1)
        {
            printf("Error : Unknown directory.");
        }
        for (i = 0; i < n;) 
        {
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
            	if(d_type == 4){
            		printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
            	}
            	else {
            		printf("%s ", namelist->d_name);
            	}
            }
            i += namelist->d_reclen;
        }
	}
	while(n > 0);
    printf("\n");
}

void ls_a(void)
{
    int fd, n, i;
    char buf[1024];
    struct linux_dirent *namelist;

    char file_name[255];
    char *file_path;
    char d_type;

    if(all_value == NULL)
    {
        file_path = ".";
    }
    else
    {
        file_path = all_value;
    }
    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do
    {
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1)
        {
            printf("Error : Unknown directory.");
        }
        for (i = 0; i < n;) {
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(d_type == 4){
                printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
            }
            else {
                printf("%s ", namelist->d_name);
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
    printf("\n");
}

void ls_l(void){
    int fd, n, i;
    char buf[1024];
    struct linux_dirent *namelist;
    struct stat fileStat;

    char file_name[255];
    char *file_path;
    char full_path[255];
    char time_buff[100];
    char d_type;

    if(l_value == NULL){
        file_path = ".";
    }
    else{
        file_path = l_value;
    }
    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        for (i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(file_name[0] !=  '.'){
                if(full_path[(strlen(full_path)-1)] == '/'){
                    strcat(full_path, namelist->d_name);
                }
                else {
                    strcat(full_path, "/");
                    strcat(full_path, namelist->d_name);
                }
                stat(full_path, &fileStat);
                // File permissions
                printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
                printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
                printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
                printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
                printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
                printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
                printf(" ");
                // User ID and group ID of file owner
                printf("%ld %ld ", (long)fileStat.st_uid, (long)fileStat.st_gid);
                // Total file size in bytes
                printf("%lld ",(long long)fileStat.st_size);
                // Last file modification
                strftime(time_buff, 100, "%b %d %H:%M", localtime(&fileStat.st_mtime));
                printf("%s ",time_buff);
                if(d_type == 4){
                    printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
                }
                else {
                    printf("%s ", namelist->d_name);
                }
                printf("\n");
            }
            i += namelist->d_reclen;
        }
    }while(n > 0);
}


void ls_al(void){
    int fd, n, i;
    char buf[1024];
    struct linux_dirent *namelist;
    struct stat fileStat;

    char file_name[255];
    char *file_path;
    char full_path[255];
    char time_buff[100];
    char d_type;

    if(l_value == NULL){
        file_path = ".";
    }
    else{
        file_path = l_value;
    }
    fd = open(file_path, O_RDONLY | O_DIRECTORY);

    do{
        n = syscall(SYS_getdents, fd, buf, 1024);
        if(n == -1){
            printf("Error : Unknown directory.");
        }
        for (i = 0; i < n;) {
            strncpy(full_path, file_path, 254);
            namelist = (struct linux_dirent *) (buf + i);
            d_type = *(buf + i + namelist->d_reclen - 1);
            strncpy(file_name, namelist->d_name, 254);
            if(full_path[(strlen(full_path)-1)] == '/'){
                strcat(full_path, namelist->d_name);
            }
            else {
                strcat(full_path, "/");
                strcat(full_path, namelist->d_name);
            }
            stat(full_path, &fileStat);
            // File permissions
            printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
            printf(" ");
            // User ID and group ID of file owner
            printf("%ld %ld ", (long)fileStat.st_uid, (long)fileStat.st_gid);
            // Total file size in bytes
            printf("%lld ",(long long)fileStat.st_size);
            // Last file modification
            strftime(time_buff, 100, "%b %d %H:%M", localtime(&fileStat.st_mtime));
            printf("%s ",time_buff);
            if(d_type == 4)
            {
                printf(BLUE BOLD    "%s "     RESET, namelist->d_name);
            }
            else 
            {
                printf("%s ", namelist->d_name);
            }
            printf("\n");

            i += namelist->d_reclen;
        }
    }
    while(n > 0);
}

// settings of the flags
while ((c = getopt_long (argc, argv, "alR", longopts, NULL)) != -1) {
	switch (c) {
	case 'a':
		all_flag = 1;
		all_value = argv[2];
		break ;
	case 'l':
		list_flag = 1;
		l_value = argv[2];
		break ;
	case 'R':
		recursive_flag = 1;
		recursive_value = argv[2];
		break ;
	case 0: /* getopt_long () set a variable , just keep going */
		break ;
	case ':': /* missing option argument */
		fprintf (stderr, "%s: option ‘-%c’ requires an argument, try --help to get the command syntax. \n",
		argv[1], optopt);
		break ;
	case '?':
	default : /* invalid option */
		fprintf (stderr, "%s: option ‘-%c’ is invalid : ignored, try --help to get the command syntax.  \n",
		argv[1], optopt);
		break ;
	case -1:
		break;
	}
}
if(all_flag == 1 && list_flag == 0 && recursive_flag == 0){
	ls_a();
}
else if(all_flag == 0 && list_flag == 1 && recursive_flag == 0){
	ls_l();
}
else if(all_flag == 1 && list_flag == 1 && recursive_flag == 0){
    ls_al();
}
else if(all_flag == 0 && list_flag == 0 && recursive_flag == 0){
	ls(argv[1]);
}
}
