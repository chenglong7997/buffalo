#ifndef virtualmem_H
#define virtualmem_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/*
**checkoption check all the arguments which use's have input.
**@argc is the number of arguments
**@argv is the array of arguments
*/
int checkoption(int argc, char *argv[]);

/*
**print_rslt print the results
**@ipolicy is the index of which policy i should use
**@page_rplc is the replacement times
**@page_ptml is the replacement times by Optimal policy
**@percentage is the penalty percentage
*/
void print_rslt(int ipolicy, unsigned long int page_rplc, unsigned long int page_ptml, float percentage);

/*
**policy_handler handle policy
**@nframe the total number of memory
**@ipolicy the index of policy.
**@ifile the file pointer to input_file
**@page_rplc the page replacement times by specific policy
**@page_ptml the page replacemet times by Optimal policy
**@percentage the penalty percentage
**@return currently has no use
*/
void policy_handler(unsigned long int nframe, int ipolicy, FILE *ifile, 
		    unsigned long int *page_rplc, unsigned long int *page_ptml, 
		    float *percentage);

/*
**provide information about all options
*/
void usage();

/*
**handle bad arguments
*/
void bad_argu();

#endif
