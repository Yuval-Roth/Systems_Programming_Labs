TARGETS = task0 task1 task2

NASM_ARGS = nasm -g -f elf32

GCC_ARGS = gcc -m32 -g -Wall -ansi -c -nostdlib -fno-stack-protector

LINKER_ARGS = ld -m elf_i386

task0: start.o main.o util.o
	$(LINKER_ARGS) start.o main.o util.o -o task0

task1: task1.o util.o
	$(LINKER_ARGS) util.o task1.o -o task1
	rm -f task1.o util.o

task2: task2.o task2c.o util.o
	$(LINKER_ARGS) util.o task2.o task2c.o -o task2
	rm -f task2.o task2c.o util.o

task1.o:
	$(NASM_ARGS) task1.asm -o task1.o

task2.o:
	$(NASM_ARGS) task2.asm -o task2.o

start.o:
	$(NASM_ARGS) start.asm -o start.o

util.o:
	$(GCC_ARGS) util.c -o util.o

task2c.o:
	$(GCC_ARGS) task2c.c -o task2c.o

main.o:
	$(GCC_ARGS) main.c -o main.o

.PHONEY: clean
clean:
	rm -f *.o $(TARGETS)
