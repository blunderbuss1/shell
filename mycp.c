#include <stdlib.h>
#include <stdio.h>
#include <utmpx.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

// return size of file named filename
off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));

    return -1;
}

// copy content of file named f1 to file named f2
void copyFile(char * f1, char * f2)
{
	// make a buffer of the right size
	char buf[fsize(f1)];
	int readf, writef;
	
	if((readf = open(f1, O_RDONLY))==-1){
    perror("open");
  }
	
	if((writef = open(f2, O_CREAT | O_RDWR, 0666))==-1){
    perror("open");
  }
  
	if(readf>0)
	{
		// read the file and put the result of the read
		// in the buffer buf
		read(readf, buf, sizeof(buf));
		printf(f2,"%s\n", buf);
		write(writef, buf, sizeof(buf));
		close(readf);
	}
	close(writef);
}

// returns if a file named fname exists
int exists(const char *fname)
{
	if (0 == access(fname, 0)) 
	{ 
		return 1;
	}
	else
	{
		return 0;
	}
}

int main(int argc, char *argv[])
{
	
	int c;
	static int backup_flag = 0;
	static int interactive_flag = 0;
	
	static struct option long_options[] =
	{
		{"backup", no_argument, &backup_flag, 'b'},
		{"interactive", no_argument, &interactive_flag, 'i'},
		{0,0,0,0}
	};

	while((c = getopt_long(argc,argv,"b:i:",long_options,NULL)) != -1)
	{
		switch(c)
		{
			case 'b':
				backup_flag = 1;
			break;
			
			case 'i':
				interactive_flag = 1;
			break;
			
			case '?':
			default:
				fprintf(stderr, "%s: option '-%c' is invalid: ignored \n",
				argv[0], optopt);
			break;
		}
	}
	
	//int numarg = argc - optind;
	//int numopt = optind - 1;
	//printf("num arg %d\n num opt %d\n backup %d\n interactive %d\n",numarg,numopt,backup_flag,interactive_flag);
	
	if((backup_flag == 1) && (interactive_flag == 0))
	{
		if(exists(argv[3]) == 1)
		{
			char backup_text[] = "BACKUP_";
			char * backup = strcat(backup_text, argv[3]);
			copyFile(argv[3],backup);
			copyFile(argv[2], argv[3]);
		}
		else
		{
			copyFile(argv[2], argv[3]);
		}
	}
	
	else if((interactive_flag == 1) && (backup_flag == 0))
	{
		if(exists(argv[3]) == 1)
		{
			printf("Do you want to overwrite %s ?[Y/N]\n", argv[3]);
			char c = getchar();
			if((c == 'Y') || (c == 'y'))
			{
				copyFile(argv[2], argv[3]);
			}
			if((c == 'N') || (c == 'n'))
			{
				return(EXIT_SUCCESS);
			}
		}
		else
		{
			copyFile(argv[2], argv[3]);
		}
	}
	
	else if((interactive_flag == 1) && (backup_flag == 1))
	{
		if(exists(argv[3]) == 1)
		{
			printf("Do you want to overwrite %s ?[Y/N]\n", argv[3]);
			char c = getchar();
			if((c == 'Y') || (c == 'y'))
			{
				// make a backup of the file named "BACKUP_" +
				// the name of the file
				char backup_text[] = "BACKUP_";
				char * backup = strcat(backup_text, argv[3]);
				copyFile(argv[3],backup);
				copyFile(argv[2], argv[3]);
			}
			if((c == 'N') || (c == 'n'))
			{
				return(EXIT_SUCCESS);
			}
		}
		else
		{
			copyFile(argv[2], argv[3]);
		}
	}
	
	else if((backup_flag == 0) || (interactive_flag == 0) 
	|| ((backup_flag == 0) && (interactive_flag == 0)))
	{
		copyFile(argv[1], argv[2]);
	}
	

	return(EXIT_SUCCESS);
}
