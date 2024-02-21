all: clean virusDetector

virusDetector: virusDetector.o
	gcc -m32 -g -Wall -o virusDetector virusDetector.o
	rm -f *.o

virusDetector.o:
	gcc -m32 -g -Wall -o virusDetector.o -c virusDetector.c

.PHONEY: clean

clean:
	rm -f *.o virusDetector
