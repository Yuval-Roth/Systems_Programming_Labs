TARGETS = task1 task4 

GCC_FLAGS = gcc -m32 -Wall -g

all: clean $(TARGETS)
	rm -f *.o

task1: task1.o
	$(GCC_FLAGS) -o task1 task1.o

task1.o: 
	$(GCC_FLAGS) -c task1.c -o task1.o

task4:
	 gcc -m32 -fno-pie -fno-stack-protector task4.c -o task4


.PHONY: clean
clean:
	rm -f $(TARGETS) *.o