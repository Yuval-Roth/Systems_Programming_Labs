#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // Include for PATH_MAX
#include "LineParser.h"
#include <sched.h>

void execute(cmdLine *pCmdLine) {
    printf("Executing command: %s\n", pCmdLine->arguments[0]);
    execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    exit(1);
}

int main() {
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
