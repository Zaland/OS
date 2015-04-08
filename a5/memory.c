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
	struct node *prev;
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
	int counter;
	header_node *head;
} memory_block;
memory_block mem;


/* Array that keeps track of the block sizes. */

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
	
	mem.counter = 0;
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
			printf("Memory leak of %d bytes at %p\n", curr->memory_size, curr->memory);
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
	/* Traverse through the block_size and find the appropriate size that matches the
	   size that is given. Example: given size = 10, best size = 16. Also check to see
	   if the size is larger than 1. After the best_size is found, check to see if the
	   best_size can be allocated. */
	  
	if(size <= 0)
	{
		printf("The size requested is too small. Such a request is not possible.\n");
		return NULL;
	}
	
	int best_size = 0, i;
	for(i = 0; i < BLOCK_LENGTH; i++)
	{
		if(block_size[i] >= size)
		{
			best_size = block_size[i];
			break;
		}
	}
	
	if(best_size > mem.free_memory)
	{
		printf("The size requested is larger than available. Such a request is not possible.\n");
		return NULL;
	}
	printf("%d %d\n", best_size, mem.free_memory);
	
	
	/* Check to see if there already is a head node for the list. If there isn't, then
       proceed to set up the head node. If not, then proceed to add it to the end of
	   the list. At the end of creating the node and referencing it, return the node. */
	   
	header_node *curr = mem.head;
	header_node *temp;
	if(curr == NULL)
	{
		temp = (header_node *)((int)(mem.memory) + sizeof(mem));
		temp->next = NULL;
		temp->prev = NULL;
		temp->memory = (void *)((int)mem.memory + best_size);
		temp->memory_size = best_size;
		temp->usage = 1;
		mem.free_memory -= best_size;
		mem.head = temp;
		mem.counter++;
		return temp;
	}
	
	else
	{
		/* If there is only a header node then proceed to create a second node and link
		   it to the head node and return the new node. Sets up the values of the current
		   node including its pointers.*/
		   
		if(curr->next == NULL)
		{
			temp = (header_node *)((int)curr + curr->memory_size + sizeof(header_node));
			temp->next = curr->next;
			temp->prev = curr;
			curr->next = temp;
			temp->memory = (void *)((int)mem.memory + best_size);
			temp->memory_size = best_size;
			mem.free_memory -= best_size;
			temp->usage = 1;
			mem.counter++;
			return temp;
		}
		
		
		/* If there are other nodes besides the header node. Then proceed to add it to the
		   end of the list. Firstly, it iterates through all the nodes to get to the most
		   recent node. Then it sets up the values of the current node including its 
		   pointers. */
		   
		else
		{			
			while(curr != NULL)
			{
				if(curr->next != NULL)
					curr = curr->next;
				else
					break;			
			}
			
			temp = (header_node *)((int)curr + curr->memory_size + sizeof(header_node));
			temp->next = curr->next;
			temp->prev = curr;
			curr->next = temp;		
			temp->memory = (void *)((int)mem.memory + best_size);
			temp->memory_size = best_size;
			mem.free_memory -= best_size;
			temp->usage = 1;
			mem.counter++;
			return temp;
		}
	}
	return NULL;
}


/* Release memory partition referenced by pointer "p" back to free space. This function
   sets the usage and removes the pointer from the list. */

void release_memory(void *p)
{
	/* Confirm the memory block p if it is null or not. If the memory block is NULL, then
	   the memory block is empty and return an error message to the screen. If the memory
	   block is not NULL, then proceed to free the memory. At the end set p to NULL. */
	
	if(p == NULL)
		printf("Memory block is empty\n");
		
	if(p != NULL)
	{		
		/* First if statement is if temp is not the first node. If temp is not the first
		   node then proceed to setting the temp's next pointer to the previous node's next
		   pointer.
		   
		   The second if statement checks to see if temp is not the last node. If temp is
		   not the last node, then proceed to setting temp's previous pointer to the next
		   node's previous pointer. */
		
		header_node *temp = (header_node *)p;

		if(temp->prev != NULL)
			temp->prev->next = temp->next;
		
		if(temp->next != NULL)
			temp->next->prev = temp->prev;
		
		/* Set the memory_size to 0. Set the next and previous pointers to NULL. Set the
		   usage to 0 to confirm that it's finished. Finally set the node itself to NULL
		   to finish the releasing of node. Also, at the end check to see if the node is 
		   NULL. If it's not NULL, then releasing the memory failed and notify the user
		   that memory was not released. */
		   
		temp->memory_size = 0;
		temp->next = NULL;
		temp->prev = NULL;
		temp->usage = 0;
		temp = NULL;
		if(temp != NULL)
			printf("Failed to release memory.\n");
	}
	p = NULL;
}


/* Print the list of the nodes using the memory_size. Prints in the forward direction and 
   then prints in the opposite direction. For testing purposes. */

void print()
{
	/* If the header node is empty, then there is nothing to printing and notify the user
	   that there is an error. */
	   
	header_node *curr = mem.head;
	int i;
	if(curr == NULL)
	{
		printf("Error printing\n");
		return NULL;
	}
	
	
	/* Prints the list by following the next pointers. These aren't ordered in any way, they
	   are unordered. */
	   
	printf("\nPrinting\n");
	for(i = 0; i < mem.counter; i++)
	{
		printf("%d", curr->memory_size);
		if(curr->next != NULL)
		{
			printf(" -> ");
			curr = curr->next;
		}
		else
			break;
	}
	printf("\n");
	
	
	/* Prints the list by following the previous pointers. These aren't ordered in any way,
	   instead they are unordered. */
	   
	for(i = 0; i < mem.counter; i++)
	{
		printf("%d", curr->memory_size);
		if(curr->prev != NULL)
		{
			printf(" -> ");
			curr = curr->prev;
		}
		else
			break;
	}
	printf("\n");
}
