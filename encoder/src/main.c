#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "IntList.c"

bool debugMode = true;
FILE *infile;
FILE *outfile;

void printDebug(char *format, char *str) {
    if(debugMode) {
        fprintf(stderr,format,str);
    }
}

void readArgs(int argc, char **argv) {
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "+D") == 0) {
            debugMode = true;
            continue;
        }
        if(strcmp(argv[i], "-D") == 0) {
            debugMode = false;
            continue;
        }
        printDebug("%s\n",argv[i]);
    }
}

void encoderLoop(){
    int encryptionIndex = 0,inputC,temp;
    size_t keySize = 1;
    char outputC;
    int *encryptionKey = (int *)malloc(4);
    encryptionKey[0] = 0;
    struct IntList list = newIntList();
    while((inputC = fgetc(infile)) != EOF){
        if(inputC == '\n'){
            printf("\n");
            encryptionIndex = 0;
            continue;
        }

        if(inputC == '+' || inputC == '-'){
            temp = fgetc(infile);
            if(temp == 'E'){
                while((inputC = fgetc(infile)) != '\n'){
                    inputC = inputC -48;
                    addLast(&list,inputC);
                }
                free(encryptionKey);
                keySize = list.size;
                encryptionKey = (int *)malloc(list.size*4);
                int i = 0 ;
                while (list.size != 0){
                    encryptionKey[i] = pop(&list);
                    i++;
                }
                continue;
            } else {
                ungetc(temp,infile);
            }
        }
        outputC = (char) (inputC + encryptionKey[encryptionIndex]);
        fputc(outputC,outfile);
        encryptionIndex = (int) ((encryptionIndex + 1) % keySize);
    }
    free(encryptionKey);
    destroyList(&list);
}

int main(int argc, char **argv) {

    readArgs(argc, argv);
    infile = stdin;
    outfile = stdout;

    encoderLoop();

    printf("\n");
    return 0;
}
