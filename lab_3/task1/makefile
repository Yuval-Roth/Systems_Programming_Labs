TARGETS = start

all: clean $(TARGETS)

NASM_ARGS = nasm -g -f elf32

GCC_ARGS = gcc -m32 -g -Wall -ansi -c -nostdlib -fno-stack-protector

LINKER_ARGS = ld -m elf_i386

start: start.o util.o
	$(LINKER_ARGS) start.o util.o -o start
	rm -f *.o

start.o:
	$(NASM_ARGS) start.s -o start.o

util.o:
	$(GCC_ARGS) util.c -o util.o

.PHONEY: clean
clean:
	rm -f *.o $(TARGETS)
