#include <stdlib.h>
#include <pwd.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

// mypwd has no options
int main(int argc, char *argv[])
{
	char * cwd;
	char buff[PATH_MAX + 1];
	// cwd to get the current working directory
	cwd = getcwd(buff, PATH_MAX+1);
	if(cwd != NULL)
	{
		printf("%s\n",cwd);
	}

  exit (EXIT_FAILURE);

}
