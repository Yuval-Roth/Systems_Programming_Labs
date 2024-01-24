CC = gcc
CFLAGS = -Wall -g
LDFLAGS =

myshell: myshell.o LineParser.o
	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o $(LDFLAGS)

myshell.o: myshell.c LineParser.h
	$(CC) $(CFLAGS) -c myshell.c

LineParser.o: LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c LineParser.c

clean:
	rm -f myshell myshell.o LineParser.o
