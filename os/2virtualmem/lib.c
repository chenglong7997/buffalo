#include "virtualmem.h"

extern flagstdin;
extern nframe;
extern ipolicy;
extern *ifile;

/*
**checkoption check all the arguments which use's have input.
**@argc is the number of arguments
**@argv is the array of arguments
*/
int checkoption(int argc, char *argv[])
{
	char file[100] = "";
	char policy[20] = "FIFO";
	//print usage summary
	if (argc == 2 && (strcmp(argv[1], "-h") == 0))
	{
		usage();
		exit(0);
	}	
	//set ipolicy according to use's input
	else if (argc == 7 || argc == 5 || argc == 3 || argc == 1)
	{
		//all options are defalut
		if (argc == 1)
			flagstdin = FALSE;
		else
		{
			int i = 1;
			for(; i < argc; i++)
			{	//set frame number
				if(strcmp(argv[i], "-f") == 0)
				{
					nframe = atoi(argv[i+1]);
					if(nframe <= 0)
					{
						printf("frame number should not less or equals than zero\n");
						exit(1);
					}
				}
				//set policy
				else if (strcmp(argv[i], "-r") == 0)
				{
					if (strcmp(argv[i+1], "FIFO") == 0)
						ipolicy = 1;
					else if (strcmp(argv[i+1], "LFU") == 0)
						{
							ipolicy = 2;
							strcpy(policy, "LFU");
						}
					else if (strcmp(argv[i+1], "SC") == 0)
						{
							ipolicy = 3;
							strcpy(policy, "SC");
						}
					else if (strcmp(argv[i+1], "LRU") == 0)
						{
							ipolicy = 4;
							strcpy(policy, "LRU");
						}
					else
						bad_argu();
				}
				//set input file
				else if (strcmp(argv[i], "-i") == 0)
				{
					if((ifile = fopen(argv[i+1], "r")) == NULL)
					{
						perror("fail open file");
						exit(1);
					}
					flagstdin = TRUE;
					strcpy(file, argv[i+1]);
				}
			}
		}
	}
	else
		bad_argu();

	if(flagstdin == TRUE)
		printf("frame number is %d, policy is %s, file is %s \n", nframe, policy, file);
	else
		printf("frame number is %d, policy is %s, page comes from STDIN\n", nframe, policy);
}

/*
**print_rslt print the results
**@ipolicy is the index of which policy i should use
**@page_rplc is the replacement times
**@page_ptml is the replacement times by Optimal policy
**@percentage is the penalty percentage
*/
void print_rslt(int ipolicy, unsigned long int page_rplc, unsigned long int page_ptml, float percentage)
{
	char policy[20];
	if(ipolicy == 1)
		strcpy(policy, "FIFO");
	if(ipolicy == 2)
		strcpy(policy, "LFU");
	if(ipolicy == 3)
		strcpy(policy, "SC");
	printf("# of page replacements with %s       :%lu\n", policy, page_rplc);
	printf("# of page replacements with Optimal    :%lu\n", page_ptml);
	printf("%c penalty using %s                   :%4.1f%\n", '%', policy, percentage);
}

/*
**provide information about all options
*/
void usage()
{
	printf("virtualmem [-h] [-f available-frames] [-r replacement-policy] [-i input_file]\n");
	printf("For example: ./virtualmen -f 10 -r LFU -i myinputfile\n");
        printf("\n");
}

/*
**handle bad arguments
*/
void bad_argu()
{
	printf("Bad arguments,please check your input...\n");
	printf("usage is as follows:\n");
	usage();
	exit(1);
}
