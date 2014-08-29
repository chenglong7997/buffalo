#ifndef policy_H
#define policy_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

//define struct Page
typedef struct PAGE
{
	unsigned long number;
	unsigned long counter;
}page;

//define struct FRAME which will be used by all policies
typedef struct FRAME
{
	bool rfrn_bit;
	unsigned long counter;
	unsigned long int iframe;
	unsigned long time;	//used to break tie
}frame;

/*
**Perror is used to printf error message and exit the process
*/
void Perror(char *msg);

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
void policy_handler(unsigned long int nframe, int ipolicy, FILE *ifile, unsigned long int *page_rplc, unsigned long int *page_ptml, float *percentage);

/*
**fifo using this function to implement FIFO policy
**@nframe is the number of total memory
**@return is the page replacement by FIFO policy
*/
unsigned long fifo(unsigned long int nframe);

/*
**lfu using this function to implement LFU policy
**@nframe is the total number of memory
**@return is the replacement times by LFU policy
*/
unsigned long lfu(unsigned long int nframe);

/*
**sc using this function to implement SC policy
**@nframe is the total number of memory
**@return is the page replacement times by SC policy
*/
unsigned long sc(unsigned long int nframe);

/*
**optimal using this function to implement Optimal policy
**@nframe is the total frame number. ifile is the file pointer to input_file.
**@return is the page replacement times by Optimal policy
*/
unsigned long optimal(unsigned long int nframe);

/*
**init_frame initialize frames. malloc memory and set init values
**@nframe is the total number of frames
**@return currently has no use
*/
int init_frame( unsigned long int nframe );

/*
**init_page, i use this function to read the page numbers in input_file to dynamic array
**@ifile is the file pointer to input_file
**@return currently has no use
*/
int init_page(FILE *ifile);

/*
**new_page is used to check whether the current page is a new page
**@number is the current page's number
**@return if it is a new page then return 1, Otherwise return 0
*/
int new_page(unsigned long number);

/*
**min_counter used to find the index which should be replaced by LFU policy
**@nframe is the the number of total memory
**@return is the index of which should be replaced
*/
unsigned long min_counter(unsigned long nframe);
/*
**check if the page page[ipage] exist in memory
**@nframe is the total number of frames.
**@ipage is the index which i want to check
**@return if the element in memory return 1, otherwise return 0
*/
int is_exist(unsigned long int nframe, unsigned long int ipage);

#endif
