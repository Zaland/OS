#About
This is a memory manager made using the binary buddy system.

#Installation
Clone to directory of preference and then run the makefile by executing `make -f Makefile`. 
Run `./test1` to run the first test case. Run `./test2` to run the second test case.

#Structures
#####`struct node`
This structure that is the header of memory blocks. Each new memory block includes this
structure and also receives space. The usage variable is 0 when this memory block 
is not in use. It will changed to 1 when the memory block is in use. The memory size
variable holds the size of the memory block. The next and previous nodes are stored 
here as well for traversing the memory blocks and for easier access to other nodes.

#####`struct memory_block`
Structure that holds information about the memory blocks initialized. Holds the 
base address of a block of memory, the size of the memory block, the amount 
of free memory. A header_node is defined here that links to the start of the
other nodes. A global structure is declared for use within this class. 

#Functions
#####`int start_memory(int size)`
Initialization for the memory module. The program will only be allowed to access
"size" bytes of memory. A program will only call this function once (in the
beginning of the program). A return value of 1 means no errors occurred while
setting up the memory module. A return value of 0 means that some error occurred
and the user can take certain actions.

#####`void end_memory`
Task-ending operations that the memory module requires. This function will also
print out which memory blocks leaked; memory is leaked when the process ends and
there was still some memory from a get_memory, grow_memory, or pregrow_memory call
that wasn't released with release_memory. A program will only call this function
once (at the end of the program).

#####`void *get_memory(int size)`
Allocate a memory partition with "size" bytes in it. Return a pointer to the memory 
space or a value of NULL if there is no space.

#####`void release_memory(void *p)`
Release memory partition referenced by pointer "p" back to free space. This function
sets the usage and removes the pointer from the list.

#####`void *grow_memory(int size, void *p)`
Try to grow the memory partition referenced by memory "p" so that it uses "size" bytes
with extra space allocated at the end of the current block of memory. Return a value
of NULL if you cannot grow the space and a pointer to the grown space if you could grow the space.

#####`void *pregrow_memory(int size, void *p)`
The same as grow_memory except that any extra space is allocated at the front of the
partition.

#####`void print(int num)`
For testing purposes. Prints in either the forward direction or backward direction
using either next or prev pointers. An int is accepted by this function to decide 
what the user wants. If num is 0, the proceed to only print forward. If num is 1,
then proceed to print backwards only. If num is 2, then print in both directions.

#Test Cases
#####Test 1 (`test1.c`)
The first test case tests the working functions and the parts where it returns error messages.

Firstly it starts the memory with a certain amount of size that could fit all the grow_memory
functions to follow. One of the grow_memory functions is larger than it should be to return 
an error message to make sure that it doesn't work. There are 4 nodes and 3 are filled in
with the all nodes being randomly chosen. The fourth node is empty.

Second, the test file will grow and pregrow memory with the already defined nodes. It will 
try to grow and pregrow memory by a larger number and try to grow. Also try to grow an empty
node that will end up returning an error. After growing each node, then print the list to
see the change.

Third, try to release memory for the nodes that are already stored. The first three nodes will
end up releasing memory, and right after it releases a memory, it will print the list using its
memory size. It will also try to release memory to the empty node which will end up returning
an error. 

Finally, it will end the memory, releasing all memory leaks and then freeing the memory that 
was malloc'ed in the beginning.



#####Test 2 (`test2.c`)
The second test adds more get memory functions and doesn't test error messages.

Firstly it starts the memory with a certain size that can fit all the get memory functions.
It will not try to return any error messages. It will store a couple of the nodes including
the head and the tail in void variables. Then it will print the current list using the
memory size as the value to be printed.

Second, the test file will grow and pregrow memory with the already defined nodes. It will 
try to grow and pregrow memory by a larger number and try to grow. Also try to grow an empty
node that will end up returning an error. After growing each node, then print the list to
see the change. It will try to pregrow the final node to a smaller number to see the change.

Third, try to release memory for the nodes that are already stored. Release all nodes in 
order and then print the list every time memory is released.

Finally, it will end the memory, releasing all memory leaks and then freeing the memory that 
was malloc'ed in the beginning. 