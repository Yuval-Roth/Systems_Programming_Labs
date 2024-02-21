all: clean bubblesort

bubblesort: bubblesort.o
	gcc -m32 -g -Wall -o bubblesort bubblesort.o
	rm -f *.o

bubblesort.o:
	gcc -m32 -g -Wall -o bubblesort.o -c bubblesort.c

.PHONEY: clean

clean:
	rm -f *.o bubblesort
