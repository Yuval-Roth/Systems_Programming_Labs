#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <unistd.h>

int debug_mode;
int openFileCount;
char* fileBuffers[2]; // Array of file buffers

typedef struct menuEntry{
    char* name;
    void (*run)();
} menuEntry;

void not_implemented(){
    printf("Not implemented yet\n");
}

void toggle_debug_mode(){
    if (debug_mode) {
        debug_mode = 0;
        printf("Debug flag now off\n");
    } else {
        debug_mode = 1;
        printf("Debug flag now on\n");
    }
}

void printElfHeader(Elf32_Ehdr *header) {
    printf("Bytes 1,2,3 of the magic number: %c%c%c\n",
           header->e_ident[EI_MAG1], header->e_ident[EI_MAG2], header->e_ident[EI_MAG3]);
    printf("Data encoding scheme: %s\n", (header->e_ident[EI_DATA] == ELFDATA2LSB) ? "Little-endian" : "Big-endian");
    printf("Entry point: 0x%x\n", header->e_entry);
    printf("Section header table offset: %u\n", header->e_shoff);
    printf("Number of section header entries: %u\n", header->e_shnum);
    printf("Size of each section header entry: %u\n", header->e_shentsize);
    printf("Program header table offset: %u\n", header->e_phoff);
    printf("Number of program header entries: %u\n", header->e_phnum);
    printf("Size of each program header entry: %u\n", header->e_phentsize);
    printf("\n");
}

/**
  In Examine ELF File, after getting a file name, open the file for reading, and then print the following:
    Bytes 1,2,3 of the magic number (in ASCII)
    Entry point (in hexadecimal)
 */
void examine_elf_file(){
    Elf32_Ehdr *header;
    int fileSize;

    if(openFileCount == 2){
        printf("There are already 2 files loaded\n");
        return;
    }

    char* file_name = (char*)malloc(100);
    // get file name
    printf("Enter file name: ");
    fgets(file_name, 100, stdin);
    file_name[strlen(file_name) - 1] = 0; // remove newline
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    // map file to memory
    // get file size
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    rewind(file);
    fileBuffers[openFileCount] = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fileno(file), 0);
    if (fileBuffers[openFileCount] == MAP_FAILED) {
        perror("mmap");
        return;
    }

    // read magic number and entry point using mmap
    header = (Elf32_Ehdr*)fileBuffers[openFileCount];
    printElfHeader(header);
    openFileCount++;
}

/**
    For each ELF file already opened by Examine ELF File, Print Section Names should visit all
    section headers in the section header table, and for each one print its index,
    name, address, offset, size in bytes, and type number.
    Note that this is done for all files currently mapped,
    so there is no file just print an error message and return.
    The format for each ELF file should be:

    File ELF-file-name
    [index] section_name section_address section_offset section_size  section_type
    [index] section_name section_address section_offset section_size  section_type
    [index] section_name section_address section_offset section_size  section_type
 */
void print_section_names() {
    int i, j;
    Elf32_Ehdr *header;
    Elf32_Shdr *shdr;
    char *strTab;

    for (i = 0; i < openFileCount; i++) {
        if (fileBuffers[i] == NULL) {
            printf("File is not loaded\n");
            continue;
        }

        header = (Elf32_Ehdr*)fileBuffers[i];
        shdr = (Elf32_Shdr*)(fileBuffers[i] + header->e_shoff);
        strTab = fileBuffers[i] + shdr[header->e_shstrndx].sh_offset;

        printf("File %d: %s\n",i+1, strTab);

        for (j = 0; j < header->e_shnum; j++) {
            printf("[%d] %s %08x %06x %06x %d\n", j,
                   strTab + shdr[j].sh_name, shdr[j].sh_addr,
                   shdr[j].sh_offset, shdr[j].sh_size, shdr[j].sh_type);
        }
        printf("\n");
    }
}

/**
  for each open ELF file, should visit all the symbols in that ELF file (if none, print an error message and return). For each symbol, print its index number, its name and the name of the section in which it is defined. (similar to readelf -s). Format should be:

    File ELF-file0name

    [index] value section_index section_name symbol_name
    [index] value section_index section_name symbol_name
    [index] value section_index section_name symbol_name
...
 */
void print_symbols() {
    int i, j, k;
    Elf32_Ehdr *header;
    Elf32_Shdr *shdrTable;
    Elf32_Sym *symbols, *entry;
    Elf32_Addr value;
    char sh_index[100];
    char *strTab, *sh_name, *symbols_names, *s_name;

    for (i = 0; i < openFileCount; i++) {
        if (fileBuffers[i] == NULL) {
            printf("File is not loaded\n");
            continue;
        }

        header = (Elf32_Ehdr *)fileBuffers[i];
        shdrTable = (Elf32_Shdr*)(fileBuffers[i] + header->e_shoff);
        strTab = fileBuffers[i] + shdrTable[header->e_shstrndx].sh_offset;

        printf("File %d: %s\n", i + 1, strTab);

        // Iterate over the sections to find symbol tables
        for (j = 0; j < header->e_shnum; j++) {
            if (shdrTable[j].sh_type == SHT_SYMTAB || shdrTable[j].sh_type == SHT_DYNSYM) {
                symbols = (Elf32_Sym *)(fileBuffers[i] + shdrTable[j].sh_offset);
                symbols_names = (char *)(fileBuffers[i] + shdrTable[shdrTable[j].sh_link].sh_offset);
                printf("\n[index] value section_index section_name symbol_name\n");

                // Iterate over the symbols in the symbol table
                // Iterate over the symbols in the symbol table
                for (k = 0; k < shdrTable[j].sh_size / sizeof(Elf32_Sym); k++) {
                    entry = &symbols[k];

                    value = entry->st_value;
                    if(entry->st_shndx == SHN_UNDEF) {
                        strcpy(sh_index, "UND");
                    } else if(entry->st_shndx == SHN_ABS) {
                        strcpy(sh_index, "ABS");
                    } else {
                        sprintf(sh_index, "%d" ,entry->st_shndx);
                    }

                    sh_name = entry->st_shndx == SHN_UNDEF || entry->st_shndx == SHN_ABS ?
                              "" : strTab + shdrTable[entry->st_shndx].sh_name;

                    s_name = entry->st_name == 0 ? "" : symbols_names + entry->st_name;

                    printf("[%02d]  %08x %-3s %-20s %-20s\n", k, value, sh_index,
                           sh_name, s_name);
                }

                printf("\n");  // Print an extra line after each symbol table
            }
        }
    }
}





int search_symbol(char *name, Elf32_Sym *symtab, int size, char *symbols_names, Elf32_Shdr *shdr1, Elf32_Ehdr *header1) {
    int i;
    for (i = 0; i < size; i++) {
        Elf32_Sym *entry = &symtab[i];
        char *symbolName = symbols_names + entry->st_name;
        if (entry->st_info == STT_SECTION) {
            int sectionIndex = entry->st_shndx;
            Elf32_Shdr *sectionHeader = &shdr1[sectionIndex];
            int sectionNameOffset = sectionHeader->sh_name;
            symbolName = (char *)(fileBuffers[0] + shdr1[header1->e_shstrndx].sh_offset + sectionNameOffset);
        }
        if (strcmp(name, symbolName) == 0) {
            if (symtab[i].st_shndx != SHN_UNDEF) {
                return 1;
            }

        }

    }
    return 0;
}



void check_files_for_merge() {
    if (openFileCount != 2) {
        printf("Need to open 2 files first\n");
        return;
    }

    Elf32_Ehdr *header1, *header2;
    Elf32_Shdr *shdr1, *shdr2;
    int i;

    header1 = (Elf32_Ehdr *)fileBuffers[0];
    header2 = (Elf32_Ehdr *)fileBuffers[1];

    shdr1 = (Elf32_Shdr *)(fileBuffers[0] + header1->e_shoff);
    shdr2 = (Elf32_Shdr *)(fileBuffers[1] + header2->e_shoff);

    int count1 = 0, count2 = 0;
    int symtab1_index = -1, symtab2_index = -1;

    for (i = 0; i < header1->e_shnum; i++) {
        if (shdr1[i].sh_type == SHT_SYMTAB || shdr1[i].sh_type == SHT_DYNSYM) {
            count1++;
            symtab1_index = i;
        }
    }

    for (i = 0; i < header2->e_shnum; i++) {
        if (shdr2[i].sh_type == SHT_SYMTAB || shdr2[i].sh_type == SHT_DYNSYM) {
            count2++;
            symtab2_index = i;
        }
    }

    if (count1 != 1 || count2 != 1) {
        printf("feature not supported\n");
        return;
    }

    //synboltable1
    Elf32_Sym *symtab1 = (Elf32_Sym *)(fileBuffers[0] + shdr1[symtab1_index].sh_offset);
    Elf32_Sym *symtab2 = (Elf32_Sym *)(fileBuffers[1] + shdr2[symtab2_index].sh_offset);
    Elf32_Sym *entry;
    char *symbols_names1 = (char *)(fileBuffers[0] + shdr1[shdr1[symtab1_index].sh_link].sh_offset);
    char *symbols_names2 = (char *)(fileBuffers[1] + shdr2[shdr2[symtab2_index].sh_link].sh_offset);


    for (i = 1; i < shdr1[symtab1_index].sh_size / sizeof(Elf32_Sym); i++) {

        entry = &symtab1[i];
        char *symbolName = symbols_names1 + entry->st_name;
        if (entry->st_info == STT_SECTION) {
            int sectionIndex = entry->st_shndx;
            Elf32_Shdr *sectionHeader = &shdr1[sectionIndex];
            int sectionNameOffset = sectionHeader->sh_name;
            symbolName = (char *)(fileBuffers[0] + shdr1[header1->e_shstrndx].sh_offset + sectionNameOffset);
        }

        if (symtab1[i].st_shndx == SHN_UNDEF && search_symbol(symbolName, symtab2, shdr2[symtab2_index].sh_size / sizeof(Elf32_Sym), symbols_names2, shdr2, header2) == 0) {
                printf("Symbol %s not defined\n", symbolName);
        }
        // else if the symbol is defined , search for it in the second table , and if defined, prinf  Symbol sym multiply defined
        else if (symtab1[i].st_shndx != SHN_UNDEF && search_symbol(symbolName, symtab2, shdr2[symtab2_index].sh_size / sizeof(Elf32_Sym), symbols_names2, shdr2, header2) == 1) {
            printf("Symbol %s multiply defined\n", symbolName);
        }
    }
}


void merge_elf_files(){
    not_implemented();
}

void quit(){
    if (debug_mode) {
        fprintf(stderr, "Debug: quitting\n");
    }
    exit(0);
}

menuEntry menu[] = {
    {"Toggle Debug Mode", toggle_debug_mode},
    {"Examine ELF File", examine_elf_file},
    {"Print Section Names", print_section_names},
    {"Print Symbols", print_symbols},
    {"Check Files for Merge", check_files_for_merge},
    {"Merge ELF Files", merge_elf_files},
    {"Quit", quit},
    {NULL, NULL}
};
int menu_size = sizeof (menu) / sizeof (menu[0]) - 1;

void print_menu(){
    int i = 0;
    //print menu
    while(menu[i].name != NULL) {
        printf("%d-%s\n", i, menu[i].name);
        i++;
    }
}

int main (int argc, char **argv){
    debug_mode = 0;
    openFileCount = 0;
    fileBuffers[0] = NULL;
    fileBuffers[1] = NULL;
    char* line = malloc(512);
    int choice;
    char *endptr;

    while(1){
        print_menu();
        fgets(line, 512, stdin);
        choice = strtol(line, &endptr, 10);
        // check if input is a number
        if (endptr == line || *endptr != '\n') {
            printf("Not a valid number\n");
            continue;
        }
        if(choice >= 0 && choice < menu_size){
            menu[choice].run();
        }
        else{
            printf("Not within bounds\n");
        }
    }
}