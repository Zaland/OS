/* Test program that tests out the memory functions. */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"


/* Main function that runs the functions defined in memory.c. */

int main(int argc, char **argv)
{
	int return_value = 0;
	void *p;
	
	start_memory(50); 
	get_memory(10);
	//void *p = get_memory(2);
	get_memory(4);
	get_memory(2);
	p = get_memory(2);
	get_memory(5);
	print();
	//release_memory(p);
	//end_memory();
	
	return return_value;
}