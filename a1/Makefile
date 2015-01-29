CCFLAGS = -I.
LDFLAGS = -lpthread -ldl

all: DSMS

DSMS: DSMS.o sqlite3.o
	gcc -o DSMS DSMS.o sqlite3.o $(LDFLAGS)

DSMS.o: DSMS.c
	gcc $(CCFLAGS) -c DSMS.c

sqlite3.o: sqlite3.c sqlite3.h sqlite3ext.h
	gcc $(CCFLAGS) -c sqlite3.c

clean:
	-rm DSMS.o sqlite3.o

spotless: clean
	-rm DSMS

