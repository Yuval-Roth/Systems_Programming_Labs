#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

int mode;

void printVirus(virus* virus, FILE* output);

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

link* list_append(link* virus_list, virus* data){

    // since we're implementing the list as a stack,
    // there is no special case for when the list is empty (null)
    link *newLink = malloc(sizeof(link));
    newLink->nextVirus = virus_list;
    newLink->vir = data;
    return newLink;
}

void freeVirus(virus *pVirus) {
    free(pVirus->sig);
    free(pVirus);
}

void list_free(link *virus_list){
    link * curr = virus_list, *next;

    while (curr != 0){
        next = curr->nextVirus;
        freeVirus(curr->vir);
        free(curr);
        curr = next;
    }
}

void list_print(link *curr, FILE* output){
    while(curr != 0){
        printVirus(curr->vir,output);
        fprintf(output,"\n");
        curr = curr->nextVirus;
    }
}

virus* readVirus(FILE *file){
    // this function receives a file pointer and returns a virus*
    // that represents the next virus in the file.
    // To read from a file, use fread().
    // See man fread(3) for assistance.
    virus *v = malloc(sizeof(virus));

    if(fread(v,sizeof(char),18,file) != 18){
        perror("fread");
    }
    if(mode < 0){
        v->SigSize = (((v->SigSize & 0xFF00) >> 8 ) | ((v->SigSize & 0x00FF) << 8 ));
    }
    v->sig = malloc(v->SigSize);
    if(fread(v->sig,sizeof(unsigned char),v->SigSize,file) != v->SigSize){
        perror("fread");
    }

    return v;
}

void printVirus(virus* virus, FILE* output){
    // this function receives a virus and a pointer to an output file.
    // The function prints the virus to the given output.
    // It prints the virus name (in ASCII),
    // the virus signature length (in decimal),
    // and the virus signature (in hexadecimal representation).
    fprintf(output, "Virus Name: %s\n", virus->virusName);
    fprintf(output,"Signature Length: %d\n",virus->SigSize);
    fprintf(output,"Signature: ");
    for (unsigned short i = 0; i < virus->SigSize; i++){
        fprintf(output,"%2X ",virus->sig[i]);
    }
    fprintf(output, "\n");
}

int legalMagicNumber(FILE *file) {

    unsigned char magicNumber[4];

    if(fread(magicNumber, sizeof(unsigned char), 4, file) != 4){
        perror("fread");
    }

    if(strcmp(magicNumber,"VIRL") == 0){
        mode = 1;
        return 1;
    } else if(strcmp(magicNumber,"VIRB") == 0) {
        mode = -1;
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char** argv){

    // open a binary file
    FILE* file = fopen("inputVIRB", "r");

    if(! legalMagicNumber(file)){
        printf("Illegal magic number");
        exit(2);
    }
    virus *v = readVirus(file);
    FILE *output = fopen("output","w");
    printVirus(v,output);

    freeVirus(v);

    fclose(file);
    fclose(output);
    return 0;


}

