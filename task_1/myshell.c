#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // Include for PATH_MAX
#include "LineParser.h"
#include <sched.h>
#include <sys/wait.h>

int debugMode = 0;

void printArray(char const *array[], int size){
    for(int i = 0; i < size; i++){
        printf("%s ",array[i]);
    }
    printf("\n");

}

void execute(cmdLine *line) {
    int child_pid = fork();
    if(child_pid == -1){
        perror("fork");
        exit(1);
    }

    if(child_pid == 0){

        if (line->inputRedirect != NULL) {
            if(freopen(line->inputRedirect, "r", stdin) == NULL){
                perror("freopen");
            }
        }

        if (line->outputRedirect != NULL) {
            if(freopen(line->outputRedirect, "w", stdout) == NULL){
                perror("freopen");
            }
        }

        execvp(line->arguments[0], line->arguments);
        _exit(1);
    } else {
        if(debugMode){
            printf("executing command: ");
            printArray((const char **) line->arguments, line->argCount);
            printf("new child pid: %d\n",child_pid);
        }
        int status;
        if(line->blocking){
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

void sendSignal(int pid, int signum) {
    // Send signal to a process
    if (kill(pid, signum) == -1) {
        perror("kill");
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

        if(userInput[0] == '\n') continue;

        // Remove the newline character from the input
        userInput[strcspn(userInput, "\n")] = '\0';

        // Exit the shell if the user enters "quit"
        if (strcmp(userInput, "quit") == 0) {
            break;
        }
        // Parse the input using parseCmdLines
        parsedCmdLine = parseCmdLines(userInput);

        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0) {
            // Change the current working directory
            if (chdir(parsedCmdLine->arguments[1]) == -1) {
                perror("chdir");
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip the execution step for "cd" command
        }

        if (strcmp(parsedCmdLine->arguments[0], "wakeup") == 0) {
            sendSignal(atoi(parsedCmdLine->arguments[1]),SIGCONT);
            continue;
        }

        if (strcmp(parsedCmdLine->arguments[0], "nuke") == 0) {
            sendSignal(atoi(parsedCmdLine->arguments[1]),SIGTERM);
            continue;
        }


        // Execute the command
        if (parsedCmdLine != NULL) {
            execute(parsedCmdLine);

            // Free the resources allocated by parseCmdLines
            freeCmdLines(parsedCmdLine);
        }
    }

    return EXIT_SUCCESS;
}
