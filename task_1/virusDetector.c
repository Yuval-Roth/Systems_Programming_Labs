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
    link *curr = virus_list;
    link *next;

    while (curr != 0){
        next = curr->nextVirus;
        freeVirus(curr->vir);
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
    virus *v = malloc(sizeof(virus));

    if(fread(v,sizeof(char),18,file) != 18){
        perror("fread");
        free(v);
        return NULL;
    }
    if(mode < 0){
        v->SigSize = (v->SigSize >> 8 ) | (v->SigSize  << 8 );
    }
    v->sig = malloc(v->SigSize);
    if(fread(v->sig,sizeof(unsigned char),v->SigSize,file) != v->SigSize){
        perror("fread");
        free(v->sig);
        free(v);
        return NULL;
    }

    return v;
}

void printVirus(virus* virus, FILE* output){

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

link *loadSignatures(char *fileName){
    char tempChar;


    FILE *file = fopen(fileName,"r");
    if(file == 0){
        perror("fopen");
        return 0;
    }
    if(!legalMagicNumber(file)){
        printf("Illegal magic number\n");
        return 0;
    }
    link *virus_list = NULL;
    while (1){
        virus *v = readVirus(file);
        if(v == NULL){
            break;
        }
        virus_list = list_append(virus_list,v);

        tempChar = fgetc(file);
        if(tempChar == EOF){
            break;
        }
        ungetc(tempChar,file);

    }
    fclose(file);
    return virus_list;
}

void detect_virus(char *buffer, unsigned int size, link *virus_list){
    link *curr = virus_list;
    while(curr != 0){
        for (unsigned int i = 0; i < size; i++){
            if(memcmp(buffer+i,curr->vir->sig,curr->vir->SigSize) == 0){
                printf("Starting byte location: %d\n",i);
                printf("Virus name: %s\n",curr->vir->virusName);
                printf("Virus size: %d\n",curr->vir->SigSize);
            }
        }
        curr = curr->nextVirus;
    }
}

void neutralize_virus(char *fileName, int signatureOffset){

    FILE *file = fopen(fileName, "r+");
    if (file == 0) {
        perror("fopen");
        return;
    }
    fseek(file, signatureOffset, SEEK_SET);
    char retInstruction = 0xc3;
    fwrite(&retInstruction, sizeof(char), 1, file);
    fclose(file);
}

int main(int argc, char** argv){
    link* virusList = NULL;

    while (1) {
        printf("1) Load signatures\n");
        printf("2) Print signatures\n");
        printf("3) Detect viruses\n");
        printf("4) Fix file\n");
        printf("5) Quit\n");

        char input[256];
        fgets(input, sizeof(input), stdin);

        int choice;
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        char buffer[10000];

        switch (choice) {
            case 1: {
                printf("Enter signature file name: ");
                char fileName[256];
                fgets(fileName, sizeof(fileName), stdin);
                fileName[strcspn(fileName, "\n")] = '\0'; // Remove the newline character if present

                virusList = loadSignatures(fileName);

                if (virusList != NULL) {
                    printf("Signatures loaded.\n");
                } else {
                    printf("Error loading signatures.\n");
                }
                break;
            }
            case 2:
                printf("Print signatures:\n");
                if (virusList != NULL) {
                    FILE *output = fopen("output.txt","w");
                    list_print(virusList,output);
                    fclose(output);
                } else {
                    printf("No signatures loaded.\n");
                }
                break;
            case 3:
                if (virusList == 0)
                {
                    printf("No signatures loaded.\n");
                    break;
                }
                if (argc < 2)
                {
                    printf("No file name provided.\n");
                    break;
                }
                FILE *file = fopen(argv[1],"r");
                if(file == 0){
                    perror("fopen");
                    break;
                }
                unsigned  int size = fread(buffer,sizeof(char),10000,file);
                if(size == 0){
                    perror("fread");
                    break;
                }

                detect_virus(buffer,size,virusList);
                fclose(file);

                break;
            case 4:
                if (virusList == 0)
                {
                    printf("No signatures loaded.\n");
                    break;
                }
                if (argc < 2)
                {
                    printf("No file name provided.\n");
                    break;
                }
                file = fopen(argv[1],"r");
                if(file == 0){
                    perror("fopen");
                    break;
                }

                size = fread(buffer,sizeof(char),10000,file);
                if(size == 0){
                    perror("fread");
                    break;
                }
                fclose(file);

                link *first = virusList;
                link *curr = virusList;

                while (curr != 0){
                    for (unsigned int i = 0; i < size; i++){
                        if(memcmp(buffer+i,curr->vir->sig,curr->vir->SigSize) == 0){
                            neutralize_virus(argv[1],i);
                        }
                    }
                    curr = curr->nextVirus;
                }
                virusList = first;
                break;
            case 5:
                list_free(virusList);
                printf("Quitting the program.\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    }

    return 0;
}

