TARGETS = loader

GCC_FLAGS = gcc -m32 -Wall -g

all: clean $(TARGETS)
	rm -f *.o

loader: loader.o
	$(GCC_FLAGS) -o loader loader.o

loader.o:
	$(GCC_FLAGS) -c loader.c -o loader.o


.PHONY: clean
clean:
	rm -f $(TARGETS) *.o