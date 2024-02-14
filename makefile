TARGETS = task0

all: clean $(TARGETS)

NASM_ARGS = nasm -f elf32

GCC_ARGS = gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector

task0: start.o main.o util.o
	ld -m elf_i386 start.o main.o util.o -o task0

start.o:
	$(NASM_ARGS) start.s -o start.o

util.o:
	$(GCC_ARGS) util.c -o util.o

main.o:
	$(GCC_ARGS) main.c -o main.o

.PHONEY: clean
clean:
	rm -f *.o $(TARGETS)
