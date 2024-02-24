TARGETS = hexeditplus

GCC_FLAGS = gcc -m32 -Wall -g

all: clean $(TARGETS)
	rm -f *.o

hexeditplus: hexeditplus.o
	$(GCC_FLAGS) -o hexeditplus hexeditplus.o

hexeditplus.o: hexeditplus.c
	$(GCC_FLAGS) -c hexeditplus.c

.PHONY: clean
clean:
	rm -f $(TARGETS) *.o