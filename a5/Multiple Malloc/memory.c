/* Custom version of the memory management functions (malloc, realloc, free...)
   using the binary buddy system. Uses 1 malloc call to function effectively. 
   The maximum size of a memory block is 16384. */
  
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "memory.h"
#define BLOCK_LENGTH (15)


/* Structure that is the header of memory blocks. Each new memory block includes this
   structure and also receives space. The usage variable is 0 when this memory block 
   is not in use. It will changed to 1 when the memory block is in use. The memory size
   variable holds the size of the memory block. The next and previous nodes are stored 
   here as well for traversing the memory blocks. */

typedef struct node
{
	int usage;
	int memory_size;
	struct node *next;
	void *memory;
} header_node;


/* Structure that holds information about the memory blocks initialized. Holds the 
   base address of a block of memory, the size of the memory block, the amount 
   of free memory. A header_node is defined here that links to the start of the
   other nodes. A global structure is declared for use within this class. */
   
typedef struct
{
	void *memory;
	int memory_size;
	int free_memory;
	header_node *head;
} memory_block;
memory_block mem;


/* Array that keeps track of the block sizes. */

/*int block_size[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68,
					72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128,
					132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180,
					184, 188, 192, 196, 200};*/
int block_size[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 
					4096, 8192, 16384};

  
/* Initialization for the memory module. The program will only be allowed to access
   "size" bytes of memory. A program will only call this function once (in the
   beginning of the program). A return value of 1 means no errors occurred while
   setting up the memory module. A return value of 0 means that some error occurred
   and the user can take certain actions. */
   
int start_memory(int size)
{
	/* Try to grab memory space and store it in the global struct mem. If the operation
	   failed, the value of that space will be NULL. Therefore, check if the value is NULL
	   before proceeding any further. If the block of space could not be allocated, then
	   return a 1 and print to the screen that space could not be allocated. */
	
	mem.memory = NULL;
	mem.memory = malloc(size);
	if(mem.memory == NULL)
	{
		printf("Failed allocating memory\n");
		return 0;
	}
	
	
	/* If memory was allocated, then we can proceed to setting global variables stored in
	   the mem structure. The memory_size variable refers to the total amount of memory 
	   available. The free_memory variable refers to the amount of free memory available. */
	   
	mem.memory_size = size;
	mem.free_memory = size;
	printf("%d %d %d\n", (int)mem.memory, mem.memory_size, mem.free_memory);
	return 1;
}


/* Task-ending operations that the memory module requires. This function will also
   print out which memory blocks leaked; memory is leaked when the process ends and
   there was still some memory from a get_memory, grow_memory, or pregrow_memory call
   that wasn't released with release_memory. A program will only call this function
   once (at the end of the program). */
   
void end_memory(void)
{
	/* Traverse through the header_nodes and see if any of the usage is still set to 1.
	   If so, then there is a memory leak. Print this memory leak to the screen and
	   release the memory. The counter keeps track of whether there were any leaks. */
	   
	int counter = 0;
	header_node *curr = mem.head;
	
	while(curr != NULL)
	{
		if(curr->usage == 1)
		{
			counter++;
			printf("Memory leak of %d bytes\n", curr->memory_size);
		}
		//header_node *temp = curr;
		curr = curr->next;
	}
	
	if(counter == 0)
		printf("No memory leaks.\n");
	
	
	/* Finally, free all the memory that was allocated by start_memory. Also check if
       the base address is NULL or not. If NULL, then the memory has already been freed.
       If not NULL, then proceed to free the memory. */
	
	if(mem.memory != NULL)
	{
		mem.head = NULL;
		mem.memory_size = 0;
		mem.free_memory = 0;
		free(mem.memory);
	}
}


/* Allocate a memory partition with "size" bytes in it. Return a pointer to the memory 
   space or a value of NULL if there is no space. */
   
void *get_memory(int size)
{
	/* Perform checks to see if the amount of memory is suitable and if there is any
	   memory left to allocate. First if statement checks for whether the size given
	   is less than 1; not possible, return NULL. The second if statement checks the
	   free_memory value to see if there is enough memory before proceeding. */
	
	if(size <= 0)
	{
		printf("The size requested is too small. Such a request is not possible.\n");
		return NULL;
	}
	
	if(size > (mem.free_memory - sizeof(header_node)))
	{
		printf("The size requested is larger than available. Such a request is not possible.\n");
		return NULL;
	}
	
	
	/* Traverse through the block_size and find the appropriate size that matches the
	   size that is given. Example: given size = 10, best size = 16. */
	   
	int best_size = 0, i;
	for(i = 0; i < BLOCK_LENGTH; i++)
	{
		if(block_size[i] >= size)
		{
			best_size = block_size[i];
			break;
		}
	}
	
	
	/* Check to see if there is any memory already allocated before. If not, then allocate
	   space and point to another header. If already allocated, then just add another 
	   header after the current header. */
	   
	header_node *curr = mem.head;
	header_node *temp;
	if(curr == NULL)
	{
		temp = (header_node *)((int)(mem.memory) + sizeof(mem));
		//temp->next = NULL;
		temp->memory = (void*)((int)mem.memory - 10);
		//temp->memory_size = best_size;
		printf("%p %d %d\n", mem.memory, best_size, (int)temp->memory);
		return temp;
	}
	printf("%d\n", (int)mem.memory - best_size);
}

