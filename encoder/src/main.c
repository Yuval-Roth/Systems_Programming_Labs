#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

bool debugMode = true;
FILE *infile;
FILE *outfile;
int *encryptionKey;
int keySize;
int additionSign;

void printDebug(char *format, char *str) {
    if(debugMode) {
        fprintf(stderr,format,str);
    }
}

void readEncryptionKey(char *key) {
    keySize = strlen(key) - 2 ;
    encryptionKey = (int *) malloc(keySize * 4);
    for(int i = 0; i < keySize; i++){
        encryptionKey[i] = key[i + 2] - ('1' - 1);
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

        if(argv[i][0] == '+' && argv[i][1] == 'E'){
            additionSign = 1;
            readEncryptionKey(argv[i]);
        } else if(argv[i][0] == '-' && argv[i][1] == 'E') {
            additionSign = -1;
            readEncryptionKey(argv[i]);
        }

        printDebug("%s\n",argv[i]);
    }
}

int moduloWithBounds(int lowerBound, int upperBound, int num) {
    int output = num;
    if(num > upperBound){
        output = lowerBound - upperBound + num -1;
    } else if(num < lowerBound){
        output = upperBound - lowerBound + num + 1;
    }
    return output;
}

void encoderLoop(){
    int encryptionIndex = 0,inputC,temp,i;
    char outputC;
    while((inputC = fgetc(infile)) != EOF){
        if(inputC == '\n'){
            printf("\n");
            continue;
        }

        temp =  (inputC + additionSign * encryptionKey[encryptionIndex]);
        if(inputC >= 'A' && inputC <= 'Z'){
            outputC = (char) moduloWithBounds('A', 'Z', temp);
        } else if(inputC >= '1' && inputC <= '9'){
            outputC = (char) moduloWithBounds('1', '9', temp);
        } else {
            outputC = (char) inputC;
        }
        fputc(outputC,outfile);
        encryptionIndex = (int) ((encryptionIndex + 1) % keySize);
    }
    free(encryptionKey);
}


int main(int argc, char **argv) {

    infile = stdin;
    outfile = stdout;

    readArgs(argc, argv);
    encoderLoop();

    printf("\n");
    return 0;
}
