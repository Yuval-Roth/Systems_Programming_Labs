all : clean encoder

encoder: main.o
	gcc -g -Wall -m32 -o encoder main.o
	rm -f main.o

main.o :
	gcc -g -Wall -m32 -c main.c -o main.o

.PHONEY : clean

clean :
	rm -f *.o encoder

