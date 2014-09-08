#include "policy.h"
#include <ctype.h>

extern int flagstdin;
int request_num = 0;

/*
**policy_handler select the corresponding function to handle the request sequence
**@ncylinder is the total number of cylinders
**@ipolicy is the index if policy
**@ifile is the file pointer to the input_file
**@result is a pointer to dynamic array for store execute sequence and their travel_num
**@travel is the total travel number
*/
void policy_handler(int ncylinder, int ipolicy, FILE *ifile, int **result, int *travel)
{
	int *list = NULL;
	init_sequence(ncylinder, ifile, &list);
	init_result(result);
	item *sorted = NULL;
	int first = list[0];

	//sort the request according to there distance to the head of disk
	if (ipolicy == 2 || ipolicy == 3 || ipolicy == 4)
		 sorted = sort(ncylinder, list);
	if (ipolicy == 1)
		*travel = fcfs(ncylinder, list, *result);
	else if (ipolicy == 2)
		*travel = sstf(first, sorted, *result);
	else if (ipolicy == 3)
		*travel = cscan(ncylinder, first, sorted, *result);
	else if (ipolicy == 4)
		*travel = look(ncylinder, first, sorted, *result);

}

/*
**sort the request sequence which pointered by list and sort the sorted sequence
**in the memory pointered by sorted
**@ncylinder is the total number of cylinders
**@list is a pointer to request sequence
**@return return the pointer to sorted sequence
*/
item *sort(int ncylinder, int *list)
{
	item *sorted = NULL;
	sorted = (item *)malloc( request_num*sizeof(item) );
	int i = 0;
	int index = 0;
	for(; i<request_num; i++)
	{
		index = min(list);
		sorted[i].request = list[index];
		sorted[i].id = index;
		sorted[i].flag = TRUE;
		list[index] = ncylinder;
	}
	return sorted;
}

/*
**min find the request which is closest to zero position in disk
**@list is a pointer to request sequence
**@return return the index to the request which is closest to zero position in disk
*/
int min(int *list)
{
	int i = 0;
	int min = list[0];
	int index = 0;
	for (; i<request_num; i++)
	{
		if (min > list[i])
		{
			min = list[i];
			index = i;
		}
	}
	return index;
}

/*
**init result array
**@result is a pointer to dynamic array for store execute sequence
*/
void init_result(int **result)
{
	//since in cscan, we should store the 0 and ncylinder-1, so add 2
	*result = (int *)malloc((request_num+2)*sizeof(int));	
	assert(*result);
	int i = 0;
	for(; i<request_num; i++)
	    (*result)[i] = 0;
}

/*
**init_sequence initialize seuqence. malloc memory and set init values
**@ncylinder is the total number of ncylinders
**@ifile is the file pointer to the input_file
**@lsit is the address of pointer to request sequence which need to be initialized
*/
void init_sequence(int ncylinder, FILE *ifile, int **list)
{
	int illegal = 0;	//used by check if it is the first illegal request
	//read page sequence from stdin
	if (flagstdin == TRUE)
	{
		int legal = 0;
		if(scanf("%d", &request_num))
			*list = (int *)malloc( request_num*sizeof(int) );
		if(*list == NULL)
			Perror("fail to realloc memory");
		int num = 0;
		int index = 0;
		int tmp = 0;
		//handle  the user's page info, store them in page
		while (index != request_num && (num = scanf("%d", &tmp)) && num != EOF && tmp != '\n' )
		{
			if(tmp < 0 || tmp >= ncylinder)
			{	
				if(illegal == 0)
				{
					printf("Illegal I/O request: ");
					illegal++;
				}
				printf("%d ", tmp);
				tmp = 0;
				request_num--;
			}
			else
			{
				(*list)[index] = tmp;
				index++;
			}
		}
	} 
	else
	{
		int c = 0;
		//if the last char is a space
		int flag = FALSE;
		//counter how many elements in file
		while((c = fgetc(ifile)) != EOF)
		{
			if( flag == FALSE && isspace( c ) )
			{
				flag = TRUE;
				++request_num;
			}
			else if( !isspace(c) )
			{
				flag = FALSE;
			}
		}
		//alloc memory
		*list = (int *)malloc( request_num*sizeof(int) );
		if(*list == NULL)
			Perror("fail to realloc memory");
		rewind(ifile);
		//read content in file to page array
		//if the last char is a space
		int tmp = 0;
		int i = 0;
		int fflage = FALSE;
		while ((c = fgetc(ifile)) != EOF)
		{
			if (!isspace(c))
			{
				tmp = c-'0'+10*tmp;
				fflage = FALSE;
			}
			else if (isspace(c) && fflage == FALSE)
			{
				fflage = TRUE;
				if (tmp < 0 || tmp >= ncylinder)      //Illegal I/O request
				{	
					if (illegal == 0)
					{
						printf("Illegal I/O request: ");
						illegal++;
					}
	 			        printf("%d ", tmp);
					tmp = 0;
				}
				else				      //legal request
				{
					(*list)[i] = tmp;
					tmp = 0;
					++i;
				}
			}
		}
		request_num -= illegal;
	}
	printf("\n");
	//print the sequence
/*	int k=0;
	for(; k<request_num; k++)
	{
		printf("%d ", (*list)[k]);
	}*/
}

/*
**fcfs using FCFS policy, execution sequence are as same as list sequence, and tracel
**are calculated by distance plus current travel num
**@ncylinder is the total number of cylinders
**@list is a pointer to request sequence
**@result is a pointer to dynamic array for store execute sequence
**@return return the travel num
*/
int fcfs(int ncylinder, int *list, int *result)
{
	printf("using fcfs...\n");
	//calculate the result by FCFS
	int i = 0;
	int travel = 0;
	for(; i<request_num; i++)	
	{
		result[i] = list[i];
		if(i != 0)
			travel += abs(list[i]-list[i-1]);
	}
	return travel;
	printf("fifo done\n");
}

/*
**look using LOOK policy
**@ncylinder is the total number of cylinders
**@first is the first request which will be executed
**@sorted is a pointer to request sequence which has been sorted
**@result is a pointer to dynamic array for store execution sequence
**@return return the travel num
*/
int look(int ncylinder, int first, item *sorted, int *result)
{
	printf("using look...\n");
	int travel = 0;
	result[0] = first;
	int index = 0;		//pointer to the current element which will be executed
	//find the first request's position in sorted array
	for (; index<request_num; index++)
	{
		if (sorted[index].request == first)
		{
			sorted[index].flag = FALSE;
			break;
		}
	}
	//handle other request in sorted array and copy the element in
	//sorted array to result array according to LOOK policy
	int left_of_first = index - 1;
	int counter = 1;
	int right = 1;
	for (; counter<request_num; counter++)
	{
		if (index + 1 == request_num)
		{
			right = 0;
			index = left_of_first;
			result[counter] = sorted[index].request;
			index--;
		}
		else if (right == 1)
		{
			index++;
			result[counter] = sorted[index].request;
		}
		else if (right == 0)
		{
			result[counter] = sorted[index].request;
			index--;
		}
	}
	//according to the execution sequence, caculate the travel
	int i = 0;
	for(; i<request_num; i++)	
	{
		if(i != 0)
			travel += abs(result[i]-result[i-1]);
	}
	return travel;
}

/*
**cscan using C-SCAN policy
**@ncylinder is the total number of cylinders
**@first is the first request which will be executed
**@sorted is a pointer to request sequence which has been sorted
**@result is a pointer to dynamic array for store execution sequence
**@return return the travel num
*/
int cscan(int ncylinder, int first, item *sorted, int *result)
{
	printf("using cscan...\n");
	int travel = 0;
	result[0] = first;
	int index = 0;		//pointer to the current element which will be executed
	//find the first request's position in sorted array
	for (; index < request_num; index++)
	{
		if (sorted[index].request == first)
		{
			sorted[index].flag = FALSE;
			break;
		}
	}
	//handle other request in sorted array and copy the element in
	//sorted array to result array according to C-SCAN policy
	int counter = 1;
	for (; counter < request_num + 2; counter++)
	{
		index++;
		//the last element in array, need return to the head of the array
		if (index == request_num)	
		{
			//store the tail and head of disk into result array
			result[counter++] = ncylinder-1;			
			result[counter++] = 0;
			index = 0;		
		}
		result[counter] = sorted[index].request;
	}
	//according to the execution sequence, caculate the travel
	int i = 0;
	for (; i < request_num + 2; i++)	
	{
		if (i != 0)
		    travel += abs(result[i]-result[i-1]);
	}
	return travel;
}

/*
**sstf using SSTF policy
**@first is the first request which will be executed
**@sorted is a pointer to request sequence which has been sorted
**@result is a pointer to dynamic array for store execution sequence
**@return return the travel num
*/
int sstf(int first, item *sorted, int *result)
{
	printf("using sstf...\n");
	int travel = 0;
	result[0] = first;
	int index = 0;		//pointer to the current element which will be executed
	//find the first request's position in sorted array
	for (; index<request_num; index++)
	{
		if (sorted[index].request == first)
		{
			sorted[index].flag = FALSE;
			break;
		}
	}
	//handle other request in sorted array and copy the element in
	//sorted array to result array according to SSTF policy
	int counter = 1;
	int left = index - 1;
	int right = index + 1;
	for (; counter<request_num; counter++)
	{
		//find the left index
		while (left != -1 && sorted[left].flag == FALSE)
			left--;
		if (left == -1)	//the left are done
		{
			//handle right
			while (sorted[right].flag == FALSE)
				right++;
			index = right;
			result[counter] = sorted[index].request;
			sorted[index].flag = FALSE;
			continue;
		}
		while (right != request_num && sorted[right].flag == FALSE)
			right++;
		if (right == request_num)	//the right are done
		{
			//handle left part
			while (sorted[left].flag == FALSE)
				left++;
			index = left;
			result[counter] = sorted[index].request;
			sorted[index].flag = FALSE;
			continue;
		}
		if (left != -1 && right != request_num)
		{
			//both side
			if (abs(sorted[index].request-sorted[left].request) > 
			    abs(sorted[index].request-sorted[right].request))
				index = right;
			else if (abs(sorted[index].request-sorted[left].request) < 
				 abs(sorted[index].request-sorted[right].request))
				index = left;
			else
			{
				if (sorted[left].id<sorted[right].id)
				    index = left;
				else
				    index = right;
			}
			result[counter] = sorted[index].request;
			sorted[index].flag = FALSE;
		}
	}
	//according to the execution sequence, caculate the travel
	int i = 0;
	for(; i < request_num; i++)	
	{
		if(i != 0)
			travel += abs(result[i]-result[i-1]);
	}
	return travel;	
}

/*
**Perror is used to printf error message and exit the process
*/
void Perror(char *msg)
{
	perror(msg);
	exit(1);
}
