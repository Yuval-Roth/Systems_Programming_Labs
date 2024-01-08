TARGET = bin/encoder

$(TARGET) : bin/main.o
	gcc -g -Wall -m32 -o $(TARGET) bin/main.o

bin/main.o :
	gcc -g -Wall -m32 -c src/main.c -o bin/main.o

.PHONEY : clean

clean :
	rm -f bin/*.o encoder

