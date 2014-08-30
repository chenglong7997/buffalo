#ifndef SIMDISK_H
#define SIMDISK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "policy.h"

#define TRUE 1
#define FALSE 0


/*
**checkoption check all the arguments which use's have input.
**@argc is the number of arguments
**@argv is the array of arguments
*/
int checkoption(int argc, char *argv[]);

/*
**@ipolicy is the index of policy
**@result is the pointer to dynamic array for store execute sequence
**@travel is the total travel number
*/
void print_rslt(int ipolicy, int *result, int travel);


/*
**provide information about all options
*/
void usage();

/*
**handle the bad arguments
*/
void bad_argu();

#endif
