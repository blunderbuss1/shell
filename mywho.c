#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <utmpx.h>
#include <getopt.h>
#include <sys/sysinfo.h>
#include <time.h>


int main(int argc, char *argv[])
{
    int heading_flag = 0;
    int count_flag = 0;

    int c;
    int nbr_user;
    char *pgr_name;
    int date_len = 100;
    char date[date_len];
    struct utmpx *user_info;
    struct tm user_lt;
    struct option long_options[] = 
    {
        {"heading", no_argument, NULL, 'H'}, 
        {"count", no_argument, NULL, 'q'}, 
        {0, 0, 0, 0}
    };

    pgr_name = argv[0];

    while ((c = getopt_long(argc, argv,"Hq",long_options, NULL)) != -1) 
    {
        switch (c) 
        {
            case 'H':
            heading_flag = 1;
            break;


            case 'q':
            count_flag = 1;
            break;

            default:
                abort();
        }
    }

    setutxent();

    nbr_user = 0;
    while ((user_info = getutxent()) != NULL) 
    {
		// if the information type is LOGIN_PROCESS (because we want to see
		// the informations about the login)
        if (user_info->ut_type == 7) 
        {
            if (count_flag) 
            {
				// print all login names and number of users logged on
                printf("%s ", user_info->ut_user);
            }
            else 
            {
                if (heading_flag && nbr_user == 0)
                {
					// print line of column headings
                    printf("NAME\tLINE\tTIME\t\t\tCOMMENT\n");
				}

                printf("%s\t", user_info->ut_user);
                printf("%s\t", user_info->ut_line);

                if (localtime_r(&user_info->ut_tv.tv_sec, &user_lt)
                 == NULL)
                    exit(EXIT_FAILURE);

                if (strftime(date, date_len, "%F %R", &user_lt) < 0)
                    exit(EXIT_FAILURE);

                printf("%s\t", date);
                printf("(%s)\t", user_info->ut_host);
                printf("\n");
            }
            nbr_user++;
        }
    }

    if (count_flag)
    {
		// print all login names and number of users logged on
        printf("\n# users=%d\n", nbr_user);
	}

    return EXIT_SUCCESS;
}
