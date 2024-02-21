all : clean base

base : base.o
	gcc -m32 -g -Wall -o base base.o
	rm -f *.o
base.o :
	gcc -m32 -g -Wall -c base.c -o base.o

.PHONEY: clean

clean:
	rm -f *.o base