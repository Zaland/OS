#ifndef _MEMORY_H_
#define _MEMORY_H_

int start_memory(int size);
void end_memory(void);
void *get_memory(int size);
void release_memory(void *p);
void *grow_memory(int size, void *p);
void *pregrow_memory(int size, void *p);
void print();

#endif
