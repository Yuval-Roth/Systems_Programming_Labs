bin/my_echo : bin/main.o
	gcc -g -Wall -m32 -o bin/my_echo bin/main.o

bin/main.o :
	gcc -g -Wall -m32 -c src/main.c -o bin/main.o

.PHONEY : clean

clean :
	rm -f bin/*.o my_echo

