all : clean menu_map

menu_map : menu_map.o
	gcc -m32 -g -Wall -o menu_map menu_map.o
	rm -f *.o
menu_map.o :
	gcc -m32 -g -Wall -c menu_map.c -o menu_map.o

.PHONEY: clean

clean:
	rm -f *.o menu_map
