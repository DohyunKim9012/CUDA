CC=gcc
CFLAGS=-g -Wall

all: tracegen dbgen 

dbgen: dbgen.c
	$(CC) -o dbgen dbgen.c $(CFLAGS)

tracegen: tracegen.c
	$(CC) -o tracegen tracegen.c $(CFLAGS)  

clean:
	rm -f *.o tracegen dbgen
