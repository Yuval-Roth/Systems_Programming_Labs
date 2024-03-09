TARGETS = loader

GCC_FLAGS = gcc -m32 -Wall -g
NASM_FLAGS = nasm -f elf32 -g
LINKER_FLAGS = ld -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

all: clean $(TARGETS)
	rm -f *.o

loader: loader.o start.o
	$(LINKER_FLAGS) -o loader loader.o start.o

loader.o:
	$(GCC_FLAGS) -c loader.c -o loader.o

start.o:
	$(NASM_FLAGS) start.s -o start.o


.PHONY: clean
clean:
	rm -f $(TARGETS) *.o