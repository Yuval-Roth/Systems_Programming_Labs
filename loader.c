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

void printPhder(Elf32_Phdr *phdr, int arg){
    printf("%-10s 0x%06x 0x%08x 0x%08x 0x%06x 0x%06x %c%c%c 0x%02x",
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

        // Print protection flags
        printf(" %s%s%s ",
               (phdr->p_flags & PF_R) ? "--- PROT_READ" : "",
               (phdr->p_flags & PF_W) ? " | PROT_WRITE" : "",
               (phdr->p_flags & PF_X) ? " | PROT_EXEC" : "");

        // Print mapping flags
        printf(" --- MAP_PRIVATE%s%s\n",
               (phdr->p_flags & PF_X) ? " | MAP_EXECUTABLE" : "",
               (phdr->p_flags & PF_W) ? " | MAP_SHARED" : "");

}

void printReadElfL(void* memory_start, int arg){
    printf("Type       Offset   VirtAddr   PhysAddr   FileSiz  MemSiz   Flg Align\n");
    foreach_phdr(memory_start, &printPhder, arg);
}

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

