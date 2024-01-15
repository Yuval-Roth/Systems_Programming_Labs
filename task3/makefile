all : clean menu

menu : menu.o
	gcc -m32 -g -Wall -o menu menu.o
	rm -f *.o
menu.o :
	gcc -m32 -g -Wall -c menu.c -o menu.o

.PHONEY: clean

clean:
	rm -f *.o menu
