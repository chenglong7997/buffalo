#include "simdisk.h"
#include "policy.h"

int ipolicy = 1;			//indicate which scheduling-policy we should use
int ncylinder = 200;
int flagstdin = TRUE;		        //defalut flagstdin is TRUE(that is read sequence number from STDIN)
FILE *ifile;			        //the input_file pointer

int main(int argc, char *argv[])
{
	int *result = NULL;	//store the executer sequence
	int travel = 0;         //store the total travel numbers
	printf("\n");
	printf("===============================================\n");
	printf("A Simple Disk Scheduling Simulator by Chenglong\n");
	printf("-----------------------------------------------\n");
	printf("\n");

	//call checkoption to handler the arguements users have input
	checkoption(argc, argv);
	//call policy_handler to handler the request
	policy_handler(ncylinder, ipolicy, ifile, &result, &travel);
	//call print_rslt to print result in specific format
	print_rslt(ipolicy, result, travel);
	
	return 0;
}
