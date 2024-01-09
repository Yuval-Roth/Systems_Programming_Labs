all : clean encoder

encoder: encoder.o
	gcc -g -Wall -m32 -o encoder encoder.o
	rm -f encoder.o

encoder.o :
	gcc -g -Wall -m32 -c encoder.c -o encoder.o

.PHONEY : clean

clean :
	rm -f *.o encoder

