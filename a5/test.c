/* Test program that tests out the memory functions. */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"


/* Main function that runs the functions defined in memory.c. */

int main(int argc, char **argv)
{
	int return_value = 0;
	
	start_memory(20); 
	get_memory(10);
	end_memory();
	
	return return_value;
}
