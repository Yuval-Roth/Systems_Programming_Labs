all: clean myshell looper

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

myshell.o: myshell.c LineParser.h
	$(CC) $(CFLAGS) -c myshell.c -o myshell.o

LineParser.o: LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o

.PHONEY: clean

clean:
	rm -f myshell looper *.o
