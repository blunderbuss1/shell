#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char ** argv_global;


/*
// stock the line written by the user in the var "line"
char * read_line()
{
	char *line = NULL;
    size_t len = 0;
    
    // fetch the line
	getline(&line, &len, stdin);
	// delimit the line with the line return character
	line=strtok(line,"\n");
	return line;
}
*/

char * read_line(int do_bash)
{
	char *line = NULL;
    size_t len = 0;
    
    // fetch the line
    if(do_bash == 0)
    {
		getline(&line, &len, stdin);
	}
	else
	{
		FILE * bash;
		bash = fopen("bash","r");
		getline(&line, &len, bash);
	}
	// delimit the line with the line return character
	line=strtok(line,"\n");
	return line;
}


// split the line read into tokens that will be the arguments/options
// of the program
char **split_line(char *line)
{
	const char *delim = " ";
	char *token;
	char **tokens = malloc(64 * sizeof(char*));
	int i = 0;
	
	token  = strtok(line, delim);
	tokens[0] = token;
	
	while(token != NULL){
		// delimit each tokens with the char delim.
		token  = strtok(NULL, delim);
		tokens[++i] = token;
	}
	return tokens;
}

// execute the programs, from their names stocked in the token with
// the arguments in the tokens
int execute(char ** args)
{
	pid_t pid = fork();
  	if(pid < 0)
  	{
		
    	// Error forking
    	printf("Error forking.\n");
    }
  	else if(pid == 0)
  	{
    	// Child process
    	if(execv(args[0], args)<0)
    	{
			printf("Error : command does not exist\n");
		}
    	//printf("%s",args[0]);
    }
  	else
  	{
    	// Parent process
    	wait(NULL);
    	// Child has finished
  	}
  	
  	return 1;
}

// fetch each line and execute it with the arguments 
// until we find the string "quit"
void shell_loop(void){
	
	char *line;
	char **args;
	int status;
	
	do
	{
		printf("sh2 > ");
		if(argv_global[1] != NULL)
		{
			line = read_line(0);
		}
		else
		{
			line = read_line(1);
		}
		if(strcmp(line,"quit") == 0)
		{
			exit(0);
		}
		args = split_line(line);
		status = execute(args);
		//printf("%s",line);

		free(line);
		free(args);
	}
	while(status);  
}

int main(int argc, char **argv)
{
	argv_global = argv;
	shell_loop();
	return EXIT_SUCCESS;
}
