#include "simdisk.h"

extern int flagstdin;
extern int ipolicy;
extern FILE *ifile;
extern int ncylinder;
int request_num;

/*
**checkoption check all the arguments which use's have input.
**@argc is the number of arguments
**@argv is the array of arguments
*/
int checkoption(int argc, char *argv[])
{
	char file[100] = "";
	char policy[20] = "FCFS";

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
			flagstdin = TRUE;
		else
		{
			int i = 1;
			for(; i < argc; i++)
			{	//set cylinder number
				if (strcmp(argv[i], "-n") == 0)
				{
					ncylinder = atoi(argv[i+1]);
					if (ncylinder <= 0)
					{
						printf("cylinder number should not less or equals than zero\n");
						exit(1);
					}
				}
				//set policy
				else if (strcmp(argv[i], "-d") == 0)
				{
					if (strcmp(argv[i+1], "FCFS") == 0)
						ipolicy = 1;
					else if (strcmp(argv[i+1], "SSTF") == 0)
					{
						ipolicy = 2;
						strcpy(policy, "SSTF");
					}
					else if (strcmp(argv[i+1], "C-SCAN") == 0)
					{
						ipolicy = 3;
						strcpy(policy, "C-SCAN");
					}
					else if( strcmp(argv[i+1], "LOOK") == 0 )
					{
						ipolicy = 4;
						strcpy(policy, "LOOK");
					}
					else
						bad_argu();
				}
				//set input file
				else if (strcmp(argv[i], "-i") == 0)
				{
					if ((ifile = fopen(argv[i+1], "r")) == NULL)
					{
						perror("fail open file");
						exit(1);
					}
					flagstdin = FALSE;
					strcpy(file, argv[i+1]);
				}
			}
		}
	}
	else
		bad_argu();
	
	if(flagstdin == FALSE)
		printf("cylinder number is %d, policy is %s, file is %s \n", ncylinder, policy, file);
	else
		printf("cylinder number is %d, policy is %s, page comes from STDIN\n", ncylinder, policy);
}

/*
**provide information about all options
*/
void  usage()
{
	printf("Usage:simdisk [-h] [-n number-of-cylinders] [-d disk-shed-policy] [-i input_file]\n");
	printf("For example: ./simdisk -n 100 -d FCFS -i myinputfile\n");
        printf("\n");
}

/*
**handle the bad arguments
*/
void bad_argu()
{
	printf("Bad arguments,please check your input...\n");
	printf("usage is as follows:\n");
	usage();
	exit(1);
}

/*
**@ipolicy is the index of policy
**@result is the pointer to dynamic array for store execute sequence
**@travel is the total travel number
*/
void print_rslt(int ipolicy, int *result, int travel)
{
	assert(result);
	char policy[10] = "";
	
	if (ipolicy == 1)
	    strcpy(policy, "FCFS");
	else if (ipolicy == 2)
	    strcpy(policy, "SSTF");
	else if (ipolicy == 3)
	{
		strcpy(policy, "C-SCAN");
		request_num += 2;
	}
	else if(ipolicy == 4)
	    strcpy(policy, "LOOK");
	printf("%s processing order: ", policy);
	
	int i = 0;
	for(; i < request_num; i++)	
		printf("%d ", result[i]);
	printf("\n");
	printf("# of cylinders traveled : %d\n", travel);
}

