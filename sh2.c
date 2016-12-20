#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int shell_option;


// stock the line written by the user in the variable named "line"
char * read_line_console()
{
	char *line = NULL;
    size_t len = 0;
    
    // fetch the line
	getline(&line, &len, stdin);
	// delimit the line with the line return character
	line=strtok(line,"\n");
	return line;
}

// return whole content of file "bash" in one string
char * read_line_file()
{
	long bash_size;
	char * bash_content;
	
	FILE* bash = fopen("bash","r");
	
	fseek(bash,0,SEEK_END);
	
	bash_size = ftell(bash);
	rewind(bash);
	
	bash_content = malloc(bash_size * (sizeof(char)));
	
	fread(bash_content, sizeof(char), bash_size, bash);
	
	fclose(bash);
	
	return bash_content;
}

// split the line read into tokens that will be the arguments/options
// of the program
char **split_line_arguments(char *line)
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

// split the content of the bash file into strings separated by "\n"
char **split_line_bash(char *line)
{
	const char *delim = "\n";
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
	char ** tokens;
	char **args;
	int status;
	int i = 0;
	
	do
	{
		printf("sh2 > ");
		if(shell_option == 0)
		{
			line = read_line_console();
			if(strcmp(line,"quit") == 0)
			{
				exit(0);
			}
			args = split_line_arguments(line);
			status = execute(args);

			free(line);
			free(args);
		}
		else if(shell_option == 1)
		{
			line = read_line_file();
			tokens = split_line_bash(line);
			do
			{
				if(strcmp(tokens[i],"quit") == 0)
				{
					exit(0);
				}
				args = split_line_arguments(tokens[i]);
				status = execute(args);
				i++;
			}
			while(tokens[i] != NULL);
		}
	}
	while(status);  
}

// if we execute the shell without any arguments, its interactive mode
// if we execute the shell with argument "bash", its bash mode and
// reads the file named "bash" in the same repertory (execute the lines
// written in the file named "bash").
int main(int argc, char **argv)
{
	if(argv[1] == NULL)
	{
		shell_option = 0;
		printf("\nYou're now in interactive mode.\n");
	}
	else 
	{
		if(strcmp(argv[1],"bash") == 0)
		{
			shell_option = 1;
			printf("\nYou're now in Bash Mode\n. Reading the file named bash...\n");
		}
	}
	shell_loop();
	return EXIT_SUCCESS;
}
