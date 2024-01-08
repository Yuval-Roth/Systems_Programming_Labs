#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool debugMode = true;

int main(int argc, char **argv) {

    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i], "+D") == 0)
        {
            debugMode = true;
            continue;
        }
        if(strcmp(argv[i], "-D") == 0){
            debugMode = false;
            continue;
        }
        if(debugMode)
        {
            fprintf(stderr,"%s\n",argv[i]);
        }
    }

    printf("\n");
    return 0;
}
