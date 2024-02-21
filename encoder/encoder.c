#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

bool debugMode;
FILE *infile;
FILE *outfile;
int *encryptionKey;
size_t keySize;
int additionSign;

void printDebug(char *format, char *str) {
    if(debugMode) {
        fprintf(stderr,format,str);
    }
}

char* substring(const char* str,size_t startIndex,size_t endIndex){
    int j = 0;
    char* substr = malloc((endIndex-startIndex+2)*sizeof(char));
    for(size_t i = startIndex; i <= endIndex ;i++,j++){
        substr[j] = str[i];
    }
    substr[j] = '\0';
    return substr;
}

size_t stringLength(const char* str){
    size_t length = 0;
    while(str[length] != '\0'){
        length++;
    }
    return length;
}

void readEncryptionKey(char *argv) {
    if (encryptionKey != 0){
        free(encryptionKey);
    }
    size_t argLength = stringLength(argv);
    char* cleanKey = substring(argv, 2, argLength-1);
    keySize = argLength - 2 ;
    encryptionKey = malloc(keySize * sizeof(int));
    for(int i = 0; i < keySize; i++){
        encryptionKey[i] = cleanKey[i] - ('1' - 1);
    }
    free(cleanKey);
}

bool argMatch(const char* arg,char* str){
    size_t length = stringLength(str);
    for (int i = 0; i < length;i++){
        if(arg[i] != str[i]) return false;
    }
    return true;
}

void readArgs(int argc, char **argv) {
    for(int i = 1; i < argc; i++){

        printDebug("%s\n",argv[i]);

        if(argMatch(argv[i],"+E")){
            additionSign = 1;
            readEncryptionKey(argv[i]);
            continue;
        }
        if(argMatch(argv[i],"-E")) {
            additionSign = -1;
            readEncryptionKey(argv[i]);
            continue;
        }
        if(argMatch(argv[i],"-I")){
            size_t argLength = stringLength(argv[i]);
            char* fileName = substring(argv[i],2,argLength-1);
            if((infile = fopen(fileName,"r")) == NULL){
                printf("opening input file failed with error code %d. defaulting to stdin\n",errno);
                infile = stdin;
                continue;
            }
            free(fileName);
            continue;
        }
        if(argMatch(argv[i],"-O")){
            size_t argLength = stringLength(argv[i]);
            char* fileName = substring(argv[i],2,argLength-1);
            if((outfile = fopen(fileName,"w")) == NULL){
                printf("opening output file failed with error code %d. defaulting to stdout\n",errno);
                outfile = stdout;
                continue;
            }
            free(fileName);
            continue;
        }
        if(argMatch(argv[i],"+D")) {
            debugMode = true;
            continue;
        }
        if(argMatch(argv[i],"-D")) {
            debugMode = false;
        }
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
    int encryptionIndex = 0,inputC;
    char outputC;
    while((inputC = fgetc(infile)) != EOF){
        if(inputC >= 'A' && inputC <= 'Z'){
            inputC += additionSign * encryptionKey[encryptionIndex];
            outputC = (char) moduloWithBounds('A', 'Z', inputC);
        } else if(inputC >= '1' && inputC <= '9'){
            inputC += additionSign * encryptionKey[encryptionIndex];
            outputC = (char) moduloWithBounds('0', '9', inputC);
        } else {
            outputC = (char) inputC;
        }
        fputc(outputC,outfile);
        fflush(outfile);
        encryptionIndex = (int) ((encryptionIndex + 1) % keySize);
    }
    free(encryptionKey);
}


int main(int argc, char **argv) {

    infile = stdin;
    outfile = stdout;
    debugMode = true;
    encryptionKey = 0;

    // default encryption key that doesn't change the output
    readEncryptionKey("+E0");

    readArgs(argc, argv);
    encoderLoop();

    printf("\n");
    return 0;
}
