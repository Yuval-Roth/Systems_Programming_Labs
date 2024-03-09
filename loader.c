#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/mman.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + header->e_phoff);
    for(int i = 0; i < header->e_phnum; i++){
        func(phdr, arg);
        phdr++;
    }
    return 0;
}


/**
    In this task you will use the iterator you created in Task 0,
    and implement the readelf -l functionality.
    Using the functions from task 0 (the iterator),
    your task is to go over the program headers in a file and for each header,
    print all the information which resides in the corresponding Elf32_Phdr structure.

    The output should look similar to readelf -l:

    Type Offset VirtAddr PhysAddr FileSiz MemSiz Flg Align
    PHDR 0x000034 0x04048034 0x04048034 0x00100 0x00100 R E 0x4
    INTERP 0x000134 0x04048134 0x04048134 0x00013 0x00013 R 0x1
    LOAD 0x000000 0x04048000 0x04048000 0x008a4 0x008a4 R E 0x1000
    LOAD 0x0008a4 0x040498a4 0x040498a4 0x0011c 0x00120 RW 0x1000
    DYNAMIC 0x0008b0 0x040498b0 0x040498b0 0x000c8 0x000c8 RW 0x4
    NOTE 0x000148 0x04048148 0x04048148 0x00020 0x00020 R 0x4
 */
void printPhder(Elf32_Phdr *phdr, int arg){
    printf("%-10s 0x%06x 0x%08x 0x%08x 0x%06x 0x%06x %c%c%c 0x%02x\n",
           (phdr->p_type == PT_LOAD) ? "LOAD" :
           (phdr->p_type == PT_INTERP) ? "INTERP" :
           (phdr->p_type == PT_NOTE) ? "NOTE" :
           (phdr->p_type == PT_DYNAMIC) ? "DYNAMIC" :
           (phdr->p_type == PT_PHDR) ? "PHDR" : "OTHER",
              phdr->p_offset, phdr->p_vaddr, phdr->p_paddr,
                phdr->p_filesz, phdr->p_memsz,
                (phdr->p_flags & PF_R) ? 'R' : ' ',
                (phdr->p_flags & PF_W) ? 'W' : ' ',
                (phdr->p_flags & PF_X) ? 'E' : ' ',
                phdr->p_align);
}

void printReadElfL(void* memory_start, int arg){
    printf("Type       Offset   VirtAddr   PhysAddr   FileSiz  MemSiz   Flg Align\n");
    foreach_phdr(memory_start, &printPhder, arg);
}

/**
    Write a program, which gets a single command line argument.
    The argument will be the file name of a 32bit ELF formatted executable.

    Your task is to write an iterator over program headers in the file.
    Implement a function with the following signature:
    int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg);

    The function arguments are:

    map_start: The address in virtual memory the executable is mapped to.
    func: the function which will be applied to each Phdr.
    arg: an additional argument to be passed to func, for later use (not used in this task).
    This function will apply func to each program header.
    Verify that your iterator works by applying it to an 32bit ELF file,
    with a function that prints out a message:
    "Program header number i at address x" for each program header i it visits.
 */
int main(int argc, char** argv){
    void* map_start;
    size_t fileSize;

    // check for correct usage
    if(argc != 2){
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    // open file
    FILE *fd = fopen(argv[1], "r");
    if(fd == 0){
        perror("fopen");
        return 1;
    }

    // calculatefile size
    fseek(fd, 0, SEEK_END);
    fileSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // copy into memory with mmap
    map_start = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fileno(fd), 0);
    if(map_start == MAP_FAILED){
        perror("mmap");
        munmap(map_start, fileSize);
        return 1;
    }

    // iterate over program headers
    printReadElfL(map_start, 0);
    return 0;
}

