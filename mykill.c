#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int c;
	int signal_flag = 0;
	int table_flag = 0;
	int signal_number;
	char *signal = "1) SIGHUP 2) SIGINT 3) SIGQUIT";
	int i = 1;

	struct option options[] = 
    {
        {"signal", required_argument, NULL, 's'},
        {"table", no_argument, NULL, 'L'},
        {0, 0, 0, 0}
	};
	
	//Stop the program if there is no argument
	if (argv[1] == 0) {
		printf("invalid argument\n");
		return 0;
	}
	
	//Flags for options
	while ((c = getopt_long(argc, argv,
        "s:L",
        options, NULL)) != -1) {
			switch (c) {
			case 's':
                signal_flag = 1;
                if (optarg == NULL){
					printf("specify signal");
					exit(EXIT_FAILURE);
				}
				signal_number = atoi(optarg);
                break;
            case 'L':
                table_flag = 1;
                break;
			default :
				abort();
			}
		}
	
	//for a simple kill()
	if ((signal_flag == 0) && (table_flag == 0)){
		while (i < argc){
			if (kill(atoi(argv[i]), 0) == 0){
				kill(atoi(argv[i]), SIGKILL);
			}
			else{
				printf("process %s does not exist\n", argv[i]);
			}
			i++;
		}
	}
	
	//for a kill -s ...
	if ((signal_flag == 1) && (table_flag == 0)){
		if (signal_number > 64){
			printf("error : check signal\n");
		}
		else{
			while (optind < argc){
				//check if the process exist
				if (kill(atoi(argv[i]), 0) == 0)
				{
					kill(atoi(argv[i]), signal_number);
				}
				else{
					printf("process %s does not exist\n", argv[i]);
				}
				optind++;
			}
		}
	}
	
	//for kill -L
	if ((signal_flag == 0) && (table_flag == 1)){
		printf("%s\n", signal);
	}
	return 0;
}
