all: clean myshell looper mypipe

CC = gcc
CFLAGS = -Wall -g -m32

looper: looper.o
	$(CC) $(CFLAGS) -o looper looper.o
	rm -f *.o

looper.o:
	$(CC) $(CFLAGS) -c looper.c -o looper.o

myshell: myshell.o LineParser.o
	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o
	rm -f *.o

myshell.o: myshell.c
	$(CC) $(CFLAGS) -c myshell.c -o myshell.o

LineParser.o: LineParser.c
	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o

mypipe : mypipe.o
	$(CC) $(CFLAGS) -o mypipe mypipe.o

mypipe.o:
	$(CC) $(CFLAGS) -c mypipe.c -o mypipe.o


.PHONEY: clean

clean:
	rm -f myshell looper mypipe *.o
