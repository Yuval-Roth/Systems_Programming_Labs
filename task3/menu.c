#include <stdio.h>
#include <stdlib.h>

char my_get(char c) {
    return (char) fgetc(stdin);
}

/* Ignores c, reads and returns a character from stdin using fgetc. */

char cprt(char c) {
    if(c >= 0x20 && c<= 0x7e){
        printf("%c\n",c);
    } else {
        printf(".\n");
    }
    return c;
}

/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */

char encrypt(char c) {
    if(c >= 0x20 && c<= 0x7e){
        return (char) (c+1);
    } else {
        return c;
    }
}

/* Gets a char c and returns its encrypted form by adding 1 to its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char decrypt(char c) {
    if(c >= 0x20 && c<= 0x7e){
        return (char) (c-1);
    } else {
        return c;
    }
}

/* Gets a char c and returns its decrypted form by reducing 1 from its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char xprt(char c) {
    printf("%x\n",c);
    return c;
}
/* xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged. */

char* map(char *array, int array_length, char (*f) (char)){
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for(int i = 0; i< array_length; i++){
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}

struct fun_desc {
    char *name;
    char (*fun)(char);
};

int main(int argc, char** argv){
    char input[5], *carray,*temp;
    carray = calloc(5,sizeof(char));

    struct fun_desc funs[] = {
            {"Get String",my_get},
            {"Print String", cprt},
            {"Print Hex",xprt},
            {"Encrypt",encrypt},
            {"Decrypt",decrypt},
            {NULL, NULL}
    };

    while(1){
        printf("Select operation from the following menu:\n");
        for(int i = 0; i < sizeof(funs) / sizeof(struct fun_desc) - 1; i++){
            printf("%d) %s\n",i,funs[i].name);
        }
        printf("Option: ");
        if(fgets(input,sizeof(input),stdin) == NULL){
            break;
        }
        if(input[0] >= 0 && input[0] <= 4){
            printf("Within bounds\n");
        } else {
            printf("Not within bounds\n");
            break;
        }
        temp = carray;
        carray = map(carray,5,funs[input[0]].fun);
        free(temp);
    }
    free(carray);
    return 0;
}

