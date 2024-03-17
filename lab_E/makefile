TARGETS = myELF
GCC_FLAGS = gcc -m32 -Wall -g



all: clean $(TARGETS)
	rm -f *.o

myELF: myELF.o
	$(GCC_FLAGS) -o myELF myELF.o

myELF.o:
	$(GCC_FLAGS) -c myELF.c -o myELF.o

.PHONY: clean
clean:
	rm -f $(TARGETS) *.o
