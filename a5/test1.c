/* Test program that tests out the memory functions. */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"


/* Main function that runs the functions defined in memory.c. Test cases are
   defined in the main function. */

int main(int argc, char **argv)
{
	int return_value = 0;
	void *p, *q, *r, *s;
	
	
	/* Start the memory and then start getting memory of various sizes. Some
	   values should be smaller than 0 and larger than the allocated space. 
	   Store some of them in pointers for releasing_memory function. */
	
	printf("********************************\n");
	printf("start_memory, and get_memory\n");
	printf("********************************\n\n");
	start_memory(200); 
	p = get_memory(10);
	get_memory(4);
	get_memory(2);
	q = get_memory(2);
	get_memory(24);
	r = get_memory(10);
	get_memory(200);
	print(0);
	
	
	/* Grow memory and pregrow memory for the void pointers that have already have
	   nodes in them. Try to increase and decrease past the limit. */
	   
	printf("\n********************************\n");
	printf("grow_memory, and pregrow_memory\n");
	printf("********************************\n\n");
	grow_memory(18, r);
	print(0);
	pregrow_memory(6, q);
	print(0);
	grow_memory(10, s);
	print(0);
	
	
	/* Print the list that has already been created before releasing memory.
	   After releasing, print the list again to see the change. Also test
	   removing one that is NULL. Test for removing head node is also included. */
	
	printf("\n********************************\n");
	printf("release_memory\n");
	printf("********************************\n\n");
	print(0);
	release_memory(p);
	print(0);
	release_memory(q);
	print(0);
	release_memory(r);
	print(0);
	release_memory(s);
	
	
	/* Finally run the end_memory function to clean up the memory. */
	
	printf("\n********************************\n");
	printf("end_memory\n");
	printf("********************************\n\n");
	end_memory();
	
	return return_value;
}
