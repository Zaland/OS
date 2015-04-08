CCFLAGS = -g

all: test1 test2

test1: test1.o memory.o
	gcc -o test1 test1.o memory.o
	
test2: test2.o memory.o
	gcc -o test2 test2.o memory.o
	
test1.o: test1.c memory.h
	gcc -c test1.c
	
test2.o: test2.c memory.h
	gcc -c test2.c
	
memory.o: memory.c memory.h
	gcc -c memory.c
	
clean:
	-rm test1.o test2.o memory.o