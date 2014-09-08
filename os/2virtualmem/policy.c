#include "policy.h"
#include <ctype.h>

extern flagstdin;
frame *list;
unsigned long page_num = 0;
page *ptpage;
unsigned long *past_page;
unsigned long past = 0;

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
		    float *percentage)
{
	//init page and frame
	init_frame(nframe);
	init_page(ifile);
	past = nframe;

	//check ipolicy and select the suitable policy
	if (ipolicy == 1)
		*page_rplc = fifo(nframe);
	else if (ipolicy == 2)
		*page_rplc=lfu(nframe);
	else if (ipolicy == 3)
		*page_rplc = sc(nframe);
	*page_ptml = optimal(nframe);
	if (flagstdin == TRUE)
		fclose(ifile);

	//caculate the percentage
	if (*page_ptml != 0)
		*percentage = (float)( (*page_rplc-*page_ptml)/(float)(*page_ptml) *100);
	else
	{
		*percentage = 0;
		printf("optimal replacement is zero\n");
	}
}

/*
**fifo using this function to implement FIFO policy
**@nframe is the number of total memory
**@return is the page replacement by FIFO policy
*/
unsigned long fifo(unsigned long int nframe)
{
	printf("using FIFO policy...\n");
	//replace times
	unsigned long rplc = 0;
	//which page needed to insert into frame list now
	unsigned long ipage = 0;
	//whether the list is full and how many elements in list
	unsigned long index = 0;
	//the index for replace
	unsigned long iget = 0;
	//handle all the page
	for(; ipage < page_num; ++ipage)
	{
		/*if page[ipage] not exist.Insert it into frame if memory has space, 
		otherwise replace a page in frame*/
		//if the page exist. is_exist function will handle it
		if (!is_exist(nframe, ipage))
		{
			//frame is not full, add page to list directly
			if(index != nframe)
			{
				list[index].iframe = ptpage[ipage].number;
				++index;
			}
			else	//select which element came first and replace it
			{
				list[iget].iframe = ptpage[ipage].number;
				iget++;
				iget = iget % nframe;
				++rplc;
			}
		}
	}
	return rplc;
}

/*
**lfu using this function to implement LFU policy
**@nframe is the total number of memory
**@return is the replacement times by LFU policy
*/
unsigned long lfu(unsigned long int nframe)
{
	printf("using lfu policy...\n");
	//replace times
	unsigned long rplc = 0;
	//timer used to break tie
	unsigned long timer = 0;
	//which page needed to insert into frame list now
	unsigned long ipage = 0;
	//whether the list is full and how many elements in list
	unsigned long index = 0;
	//the index for replace
	unsigned long iget = 0;
	//handle all the page
	for (; ipage < page_num; ++ipage)
	{
		/*if page[ipage] not exist.Insert it in frame if memory has space, 
		otherwise replace a page in frame*/
		//if the page exist.iso_exist function will handle it
		if (!is_exist(nframe, ipage))
		{
			/*frame is not full, add page to list directly,add this 
			page to past_page list,set counter to 1 and add the current time */
			if (index != nframe)
			{
				list[index].iframe = ptpage[ipage].number;
				past_page[index] = ptpage[ipage].number;
				ptpage[ipage].counter = 1;
				list[index].counter = ptpage[ipage].counter;
				list[index].time = timer++;
				++index;
			}
			else	//select which element's counter is smallest and replace it
			{
				//find index which should be replaced
				iget = min_counter(nframe);
				//we should add this page to list[iget]
				//calculate this page's counter
				if (1 == new_page(ptpage[ipage].number))
				{
					ptpage[ipage].counter = 1;
					past_page[past++] = ptpage[ipage].number;
				}
				/*ptpage[ipage] is an old page, then increase 
				all the corresponding old page's counter by one, and copy it to page struct*/
				else
				{
					unsigned long i = 0;
					for(; i<ipage; i++)
					{
						if(ptpage[i].number == ptpage[ipage].number)
							ptpage[ipage].counter = ++ptpage[i].counter;
					}
				}
				//add page to frame and set frame's counter and time
				list[iget].iframe = ptpage[ipage].number;
				list[iget].counter = ptpage[ipage].counter;
				list[iget].time = timer++;
				++rplc;
			}	
		}
	}
	return rplc;
}

/*
**min_counter used to find the index which should be replaced by LFU policy
**@nframe is the the number of total memory
**@return is the index of which should be replaced
*/
unsigned long min_counter(unsigned long nframe)
{
	unsigned long iget = 0;
	unsigned long i = 0;
	unsigned long first = 0;
	unsigned long counter = 0;
	for(; i < nframe; i++)
	{
		if(first == 0 && list[i].counter>counter)
		{
			counter = list[i].counter;
			iget = i;
			first = 1;
		}
		else if(first == 1 && list[i].counter<counter)
		{
			counter = list[i].counter;
			iget = i;
		}
		/*if there are some item have the same smallest counter, then we 
		choose whose time is littlest. Then means choose the job which is oldest. Like FIFO*/
		else if(first == 1 && list[i].counter == counter && list[i].time<list[iget].time)	
			iget = i;
	}
	return iget;
}

/*
**new_page is used to check whether the current page is a new page
@number is the current page's number
@return if it is a new page then return 1, Otherwise return 0
*/
int new_page(unsigned long number)
{
	unsigned long i = 0;
	for(; i < past; i++)
		if(past_page[i] == number)
			return 0;
	
	return 1;		//new page return 1
}

/*
**sc using this function to implement SC policy
**@nframe is the total number of memory
**@return is the page replacement times by SC policy
*/
unsigned long sc(unsigned long int nframe)
{
	printf("using sc policy...\n");
	//replace times
	unsigned long rplc = 0;
	//which page needed to insert into frame list now
	unsigned long ipage = 0;
	//how many elements in list
	unsigned long index = 0;
	//the index for replace
	unsigned long iget = 0;
	for (; ipage<page_num; ++ipage)
	{
		/*if page[ipage] not exist.Insert it in frame if memory has a 
		 * room, otherwise replace a page in frame*/
		//if the page exist. is_exit function will handle it
		if (!is_exist(nframe, ipage))
		{
			//frame is not full, add page to list directly and set reference bit to FALSE
			if (index != nframe)
			{
				list[index].iframe = ptpage[ipage].number;
				list[index].rfrn_bit = FALSE;
				++index;
			}
			else	
		       /*select which element's reference's bit equals FALSE , 
			* replace it and set new page's reference bit to FALSE*/
			{
				while (1)
				{
					if (list[iget].rfrn_bit == FALSE)
					{
						list[iget].iframe = ptpage[ipage].number;
						list[iget].rfrn_bit = FALSE;
						iget++;
						iget = iget % nframe;
						break;
					}
					else	
				        /*if reference bit equals to TRUE, 
				        set it equals to FALSE, and move pointer to next*/
					{
						list[iget].rfrn_bit = FALSE;
						iget++;
						iget = iget % nframe;
				        }
				}
				++rplc;
			}	
		}
	}
	return rplc;
}

/*
**optimal using this function to implement Optimal policy
**@nframe is the total frame number. ifile is the file pointer to input_file.
**@return is the page replacement times by Optimal policy
*/
unsigned long optimal(unsigned long int nframe)
{
	printf("using optimal policy...\n");
	//clear the list
	unsigned long i = 0;
	for(; i<nframe; ++i)
	{
		list[i].iframe = 0;
		list[i].counter = 0;
		list[i].rfrn_bit = FALSE;
		list[i].time = 0;
	}
	//replace times
	unsigned long rplc = 0;
	//which page needed to insert into frame list now
	unsigned long ipage = 0;
	//whether the list is full and how many elements in list
	unsigned long index = 0;
	//the index for replace
	unsigned long iget = 0;
	for (; ipage < page_num; ++ipage)
	{
		/*if page[ipage] not exist.Insert it in frame if memory has a room, 
		 * otherwise replace a page in frame*/
		//if the page exist. is_exit function will handle it
		if(!is_exist(nframe, ipage))
		{
			//frame is not full, add page to list directly
			if(index != nframe)
			{
				list[index].iframe = ptpage[ipage].number;
				++index;
			}
			else	//Replace page that will not be used for longest period of time
			{
				iget = 0;
				unsigned long longest = 0;
				unsigned long iframeindex = 0;
				for(; iget < nframe; ++iget)
				{
					unsigned long i = ipage+1;
					//find the page that will not be used fot longest time
					for(; i < page_num; ++i)
					{
						if (list[iget].iframe == ptpage[i].number)
						{	
							if(i > longest)
							{
								longest = i;
								iframeindex = iget;
							}
							break;
						}
					}
				        //since i equals page_num,so page[i] will not used in furture,record iget
					if(i == page_num)	
					{
						iframeindex = iget;
						break;
					}
				}
				list[iframeindex].iframe = ptpage[ipage].number;
				++rplc;
			}
		}
	}
	return rplc;
}

/*
**init_frame initialize frames. malloc memory and set init values
**@nframe is the total number of frames
**@return currently has no use
*/
int init_frame( unsigned long int nframe )
{
	list = (frame *)malloc( nframe*sizeof(frame) );
	if(list == NULL)
		Perror("fail to malloc for frame list");
	unsigned long i = 0;
	for(; i<nframe; i++)
	{
		list[i].iframe = 0;
		list[i].counter = 0;
		list[i].rfrn_bit = FALSE;
		list[i].time = 0;
	}	
}

/*
**init_page, i use this function to read the page numbers in input_file to dynamic array
**@ifile is the file pointer to input_file
**@return currently has no use
*/
int init_page(FILE *ifile)
{
	//read page sequence from stdin
	if(flagstdin == FALSE)
	{
		unsigned long tmp = 0;
		unsigned long index = 0;
		//the first item is the page number
		if(scanf("%lu", &page_num))
			ptpage = (page *)malloc(page_num*sizeof(page));
			past_page = (unsigned long *)malloc( page_num*sizeof(unsigned long) );
		if(ptpage == NULL || past_page == NULL)
				Perror("fail to realloc memory");
		int num = 0;
		//handle  the user's page info, store them in page
		while (index != page_num && (num = scanf("%lu", &tmp)) && num != EOF && num != 0 && tmp != '\n')
		{	
			ptpage[index].number = tmp;
			ptpage[index].counter = 0;
			index++;
		}
		unsigned long i = 0;
		for(; i<page_num; i++)
			past_page[i] = 0;
	}
	//read page sequence from file
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
				++page_num;
			}
			else if( !isspace(c) )
			{
				flag = FALSE;
			}
		}
		ptpage = (page *)malloc(page_num*sizeof(page));
		past_page = (unsigned long *)malloc( page_num*sizeof(unsigned long) );
		if(ptpage == NULL)
			Perror("fail to malloc for page array");
		//init the page
		unsigned long k = 0;
		for(; k<page_num; k++)
		{
			ptpage[k].number = 0;
			ptpage[k].counter = 0;
		}
		unsigned long i = 0;
		for(; i<page_num; i++)
			past_page[i] = 0;
		//assign values
		i = 0;
		unsigned long tmp = 0;
		rewind(ifile);
		//read content in file to page array
		//if the last char is a space
		int fflage = FALSE;
		while ((c = fgetc(ifile)) != EOF)
		{
			if (!isspace(c))
			{
				tmp = c - '0' + 10 * tmp;
				fflage = FALSE;
			}
			else if (isspace(c) && fflage == FALSE)
			{
				fflage = TRUE;
				ptpage[i].number = tmp;
				tmp = 0;
				++i;
			}
		}
	}
}

/*
**check if the page page[ipage] exist in memory
**@nframe is the total number of frames.
**@ipage is the index which i want to check
**@return if the element in memory return 1, otherwise return 0
*/
int is_exist(unsigned long int nframe, unsigned long int ipage)
{
	unsigned long i = 0;
	for(; i<nframe; ++i)
	{
		if(list[i].iframe == ptpage[ipage].number)	//the element has existed!
		{
			list[i].rfrn_bit = TRUE;
			unsigned long j = 0;
			for(; j<ipage; j++)
			{
				//find this page last time's counter, increase it by one
				if(ptpage[j].number == ptpage[ipage].number)
					ptpage[ipage].counter = ++ptpage[j].counter;	
			}
			list[i].counter = ptpage[ipage].counter;
			return 1;
		}
	}
	return 0;
}

/*
**Perror is used to printf error message and exit the process
*/
void Perror(char *msg)
{
	perror(msg);
	exit(1);
}

