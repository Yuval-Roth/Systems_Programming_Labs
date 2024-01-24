#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // Include for PATH_MAX
#include "LineParser.h"
#include <sched.h>
#include <sys/wait.h>


int debugMode = 0;

void execute(cmdLine *pCmdLine) {
    int child_pid = fork();
    if(child_pid == -1){
        perror("fork");
        exit(1);
    }
    
    if(child_pid == 0){
        if(debugMode){
            char pString[256] = pCmdLine->arguments;
            printf("executing command: %s\n",pString);
        }
        execvp(pCmdLine->arguments[0],pCmdLine->arguments);
        _exit(1);
    } else {
        if(debugMode){
            printf("new child pid: %d\n",child_pid);
        }
        int status;
        if(pCmdLine->blocking){
            waitpid(child_pid,&status,0);
        }
    }

}

void readArgs(int argc, char **argv) {
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i],"-d") == 0) {
            debugMode = 1;
        }
    }
}

int main(int argc, char** argv) {

    readArgs(argc,argv);

    char userInput[2048];
    cmdLine *parsedCmdLine;

    while (1) {
        // Display the current working directory as the prompt
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

        // Print the prompt
        printf("%s$ ", cwd);

        // Read a line from the user
        if (fgets(userInput, sizeof(userInput), stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }

        // Remove the newline character from the input
        userInput[strcspn(userInput, "\n")] = '\0';

        // Exit the shell if the user enters "quit"
        if (strcmp(userInput, "quit") == 0) {
            break;
        }

        // Parse the input using parseCmdLines
        parsedCmdLine = parseCmdLines(userInput);

        // Execute the command
        if (parsedCmdLine != NULL) {
            execute(parsedCmdLine);

            // Free the resources allocated by parseCmdLines
            freeCmdLines(parsedCmdLine);
        }
    }

    return EXIT_SUCCESS;
}
