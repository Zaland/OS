CCFLAGS = -g

all: test1

test1: test1.o memory.o
	gcc -o test1 test1.o memory.o
	
test1.o: test1.c memory.h
	gcc -c test1.c
	
memory.o: memory.c memory.h
	gcc -c memory.c
	
clean:
	-rm test1.o memory.o