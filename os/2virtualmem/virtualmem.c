#include "virtualmem.h"

unsigned long int nframe = 5;			//the number of available-frames
int ipolicy = 1;		                //indicate which replacement-policy we should use
int flagstdin = FALSE;		                //defalut flagstdin is FALSE(that is read from stdin)
FILE *ifile;			                //the input_file pointer

int main(int argc, char *argv[])
{	
	//page_rplc the page replacements with user's policy
	//page_ptml the page replacements with optimal policy
	unsigned long int page_rplc=0;
	unsigned long page_ptml=0;
	//penalty using user's policy
	float percentage=0.0;
	
	printf("\n");
	printf("===============================================\n");
	printf("A Simple Vritual Memory Manage by Chenglong\n");
	printf("-----------------------------------------------\n");
	printf("\n");

	//call checkoption to handler the arguements users have input
	checkoption(argc, argv);
	//call policy_handler to handler the request
	policy_handler(nframe, ipolicy, ifile, &page_rplc, &page_ptml, &percentage);
	//call print_rslt to print result in good format
	print_rslt(ipolicy, page_rplc, page_ptml, percentage);
	
	return 0;
}
