test1.c
----------------------------------------------------------------------------------------------

The first test case tests the working functions and the parts where it returns error messages.

Firstly it starts the memory with a certain amount of size that could fit all the grow_memory
functions to follow. One of the grow_memory functions is larger than it should be to return 
an error message to make sure that it doesn't work. There are 4 nodes and 3 are filled in
with the first node being the head, and the other two nodes are randomly chosen. The fourth
node is empty to confirm its function when used.

Second, the test file will grow and pregrow memory with the already defined nodes. It will 
try to grow and pregrow memory by a larger number and try to grow. Also try to grow an empty
node that will end up returning an error.

Third, try to release memory for the nodes that are already stored. The first three nodes will
end up releasing memory, and right after it releases a memory, it will print the list using its
memory size. It will also try to release memory to the empty node which will end up returning
an error. 

Finally, it will end the memory, releasing all memory leaks and then freeing the memory that 
was malloc'ed in the beginning.