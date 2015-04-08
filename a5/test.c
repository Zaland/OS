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
	   
	start_memory(50); 
	p = get_memory(10);
	get_memory(4);
	get_memory(2);
	q = get_memory(2);
	get_memory(24);
	r = get_memory(10);
	
	
	/* Print the list that has already been created before releasing memory.
	   After releasing, print the list again to see the change. Also test
	   removing one that is NULL. */
	   
	print(0);
	release_memory(p);
	print(0);
	release_memory(q);
	print(0);
	release_memory(r);
	print(0);
	release_memory(s);
	
	
	/* Finally run the end_memory function to clean up the memory. */
	
	end_memory();
	
	return return_value;
}
