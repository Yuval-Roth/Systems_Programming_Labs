#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

// this function receives a file pointer and returns a virus*
// that represents the next virus in the file.
// To read from a file, use fread().
// See man fread(3) for assistance.
virus* readVirus(FILE *file){
    unsigned char magicNumber[4];
    virus *v = malloc(sizeof(virus));
    unsigned short *sizeBuf = 0,tempShort;
    char *nameBuf = 0 ,tempChar;
    unsigned char *sigBuf = 0,tempUChar;

    if(fread(magicNumber, sizeof(unsigned char), 4, file) != 4){
        perror("fread");
    }

    sizeBuf = malloc(2*sizeof(unsigned  short));
    if (fread(sizeBuf, sizeof(unsigned  short ), 1, file) != 1){
        perror("fread");
    }

    nameBuf = calloc(16,sizeof(char));
    if (fread(nameBuf, sizeof(char), 16, file) != 16){
        perror("fread");
    }

    sigBuf = malloc((int)sizeBuf*sizeof(unsigned char));
    if(fread(sigBuf, (int)sizeBuf*sizeof(unsigned char), (int)sizeBuf, file) != v->SigSize){
        perror("fread");
    }

    if (strcmp(magicNumber,"VIRB") == 0){
        //BIG ENDIAN

        //flip sigSize
        tempShort = sizeBuf[0];
        sizeBuf[0] = sizeBuf[1];
        sizeBuf[1] = tempShort;

        //flip nameBuf
        for (int i = 0; i < 16; ++i) {
            tempChar = nameBuf[i];
            nameBuf[i] = nameBuf[15-i];
            nameBuf[15-i] = tempChar;
        }

        //flip sigBuf
        for (int i = 0; i < v->SigSize; ++i) {
            tempUChar = sigBuf[i];
            sigBuf[i] = sigBuf[v->SigSize-1-i];
            sigBuf[v->SigSize-1-i] = tempUChar;
        }
    }

    // assign from buffers
    v->sig = sigBuf;
    strcpy(v->virusName,nameBuf);
    v->SigSize = *sizeBuf;

    return v;
}


// this function receives a virus and a pointer to an output file.
// The function prints the virus to the given output.
// It prints the virus name (in ASCII),
// the virus signature length (in decimal),
// and the virus signature (in hexadecimal representation).
void printVirus(virus* virus, FILE* output){

}

int main(int argc, char** argv){

    // open a binary file
    FILE* file = fopen("input", "r");

    // Test the readVirus function
    virus* testVirus = readVirus(file);
    if (testVirus != NULL) {
        // Print the virus details
        printf("Signature Size: %u\n", testVirus->SigSize);
        printf("Virus Name: %s\n", testVirus->virusName);
        printf("Virus Signature: ");
        for (unsigned short i = 0; i < testVirus->SigSize; ++i) {
            printf("%02X ", testVirus->sig[i]);
        }
        printf("\n");

        // Free the allocated memory
        free(testVirus->sig);
        free(testVirus);
    } else {
        printf("Error reading virus from file\n");
    }

    // Close the temporary file
    fclose(file);

    return EXIT_SUCCESS;

}