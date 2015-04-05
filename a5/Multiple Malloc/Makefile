CCFLAGS = -g

test: test.o memory.o
	gcc -o test test.o memory.o
	
test.o: test.c memory.h
	gcc -c test.c
	
memory.o: memory.c memory.h
	gcc -c memory.c
	
clean:
	-rm test.o memory.o