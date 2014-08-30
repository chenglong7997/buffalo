#ifndef POLICY_H
#define POLICY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0

typedef struct ITEM
{
	int id;		//the id of coming sequence
	int request;	
	int flag;	//indicate whether the request has been served
}item;

/*
**policy_handler select the corresponding function to handle the request sequence
**@ncylinder is the total number of cylinders
**@ipolicy is the index if policy
**@ifile is the file pointer to the input_file
**@result is a pointer to dynamic array for store execute sequence and their travel_num
**@travel is the total travel number
*/
void policy_handler(int ncylinder, int ipolicy, FILE *ifile, int **result, int *travel);

/*
**init result array
**@result is a pointer to dynamic array for store execute sequence
*/
void init_result(int **result);

/*
**init_sequence initialize seuqence. malloc memory and set init values
**@ncylinder is the total number of ncylinders
**@ifile is the file pointer to the input_file
**@lsit is the address of pointer to request sequence which need to be initialized
*/
void init_sequence(int ncylinder, FILE *ifile, int **list);

/*
**fcfs using FCFS policy, execution sequence are as same as list sequence, and tracel
**are calculated by distance plus current travel num
**@ncylinder is the total number of cylinders
**@list is a pointer to request sequence
**@result is a pointer to dynamic array for store execute sequence
**@return return the travel num
*/
int fcfs(int ncylinder, int *list, int *result);

/*
**sort the request sequence which pointered by list and sort the sorted sequence
**in the memory pointered by sorted
**@ncylinder is the total number of cylinders
**@list is a pointer to request sequence
**@return return the pointer to sorted sequence
*/
item *sort(int ncylinder, int *list);

/*
**min find the request which is closest to zero position in disk
**@list is a pointer to request sequence
**@return return the index to the request which is closest to zero position in disk
*/
int min(int *list);

/*
**sstf using SSTF policy
**@first is the first request which will be executed
**@sorted is a pointer to request sequence which has been sorted
**@result is a pointer to dynamic array for store execution sequence
**@return return the travel num
*/
int sstf(int first, item *sorted, int *result);

/*
**cscan using C-SCAN policy
**@ncylinder is the total number of cylinders
**@first is the first request which will be executed
**@sorted is a pointer to request sequence which has been sorted
**@result is a pointer to dynamic array for store execution sequence
**@return return the travel num
*/
int cscan(int ncylinder, int first, item *sorted, int *result);

/*
**look using LOOK policy
**@ncylinder is the total number of cylinders
**@first is the first request which will be executed
**@sorted is a pointer to request sequence which has been sorted
**@result is a pointer to dynamic array for store execution sequence
**@return return the travel num
*/
int look(int ncylinder, int first, item *sorted, int *result);

/*
**Perror is used to printf error message and exit the process
*/
void Perror(char *msg);

#endif
