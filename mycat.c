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

static int nlines = 0; // number of lines

// return size of file named filename
// using structure stat
off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));

    return -1;
}

// split a file string into multiple strings (the lines)
char **split_line(char *line)
{
	const char *delim = "\n";
	char *token;
	char **tokens = malloc(64 * sizeof(char*));
	int i = 0;
	
	token  = strtok(line, delim);
	tokens[0] = token;
	
	while(token != NULL)
	{
		token  = strtok(NULL, delim);
		tokens[++i] = token;
		nlines++;
	}
	
	return tokens;
}

// append content of file named f1 to file named f2
// (with O_APPEND)
void appendContentofFile1toFile2(char * f1, char * f2)
{
	int buf_length;
	int readf, writef;
	
	if((readf = open(f1, O_RDONLY))==-1){
	perror("open");}
	buf_length = fsize(f1);
	
	char buf[buf_length];
	
	if((writef = open(f2, O_CREAT | O_APPEND | O_RDWR, 0666))==-1){
    perror("open");
  }
  
	if(readf>0)
	{
		read(readf, buf, sizeof(buf));
		printf(f2,"%s\n", buf);
		write(writef, buf, sizeof(buf));
		close(readf);
	}
	close(writef);
}

// Display the content of the file named f1 into the command line output.
void displayFileContent(char * f1, int option)
{
	// option = 0 when no flags activated
	// option = 1 when display_line_number_flag = 1
	// option = 2 when show_ends_flags = 1
	// option = 3 when both flags are activated
	int buf_length = fsize(f1);
	char buf[buf_length];
	int readf;
	int i = 0;
	char end = '$';
	
	if((readf = open(f1, O_RDONLY))==-1){
    perror("open");
  }
  
	if(readf>0)
	{
		read(readf, buf, sizeof(buf));
		if(option == 0) // if no option we just display the file content
		{
			printf("%s\n", buf);
		}
		else if(option > 0)
		{
			char ** lines = split_line(buf);
			if(option == 1) // if we want to display line numbers
			{
				for(i=0; i < nlines; i++)
				{
					printf("%d.%s\n",i,lines[i]);
				}
			}
			else if(option == 2) // if we want to display line numbers
			{	
				for(i=0; i < nlines; i++)
				{
					printf("%c  %s\n",end,lines[i]);
				}
			}
			else if(option == 3) // if we want to display line numbers
			{
				for(i=0; i < nlines; i++)
				{
					printf("%c  %d.%s\n",end,i,lines[i]);
				}
			}
			close(readf);
		}
	}
}

int main(int argc, char *argv[])
{
  static int display_line_number_flag = 0; // flag n
  static int show_ends_flag = 0; // flag E
  int c;
  
	static struct option long_options[] =
	{
		{"number", no_argument, &display_line_number_flag, 'n'},
		{"show-ends", no_argument, &show_ends_flag,'E'},
		{0,0,0,0}
	};

	while((c = getopt_long(argc,argv,"n:E:",long_options,NULL)) != -1)
	{
		switch(c)
		{
			case 'n':
			display_line_number_flag = 1;
			break;
			
			case 'E':
			show_ends_flag = 1;
			break;
			
			case '?':
			default:
				fprintf(stderr, "%s: option '-%c' is invalid: ignored \n",argv[0], optopt);
			break;
		}
	}
	int nb_args = argc - optind;
	//int nb_options = optind - 1;
	//printf("nb arg %d\n",nb_args);
	//printf("nb opt %d\n",nb_options);
		
	if(nb_args < 3) // display file content 
	{
		if((display_line_number_flag == 0) && (show_ends_flag == 0))
		{
			// display file content only
			displayFileContent(argv[1], 0);
		}
		else if((display_line_number_flag == 1) && (show_ends_flag == 0))
		{
			// display file content and show line numbers
			displayFileContent(argv[2], 1);
		}
		else if((show_ends_flag == 1) && (display_line_number_flag == 0))
		{
			// display file content with end of line characters '$'
			displayFileContent(argv[2], 2);
		}
		else if((display_line_number_flag == 1) && (show_ends_flag == 1))
		{
			// display file content with line numbers and with
			// end of line characters
			displayFileContent(argv[2], 3);
		}
	}
	
	if(nb_args == 3) // concatenate files
	{	
		appendContentofFile1toFile2(argv[1],argv[3]);
		appendContentofFile1toFile2(argv[2],argv[3]);	
		return(0);
	}
  
	exit (EXIT_SUCCESS);
}
