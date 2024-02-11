#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "LineParser.h"
#include <sys/wait.h>
#include <errno.h>

#define TERMINATED  (-1)
#define RUNNING 1
#define SUSPENDED 0

typedef struct process process;
typedef struct cmdLine cmdLine;
typedef struct history history;


// <--- Global Variables --->

int debugMode = 0;
process *processes = NULL;
history shellHistory;

// <--- Struct Declarations --->

typedef struct history{
    char *inputs[20];
    int historySize;
    int firstIndex;
} history;

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

// <--- History Functions --->

char* getHistoryRecord(int index){

    index--; // convert to 0 based index because the user will enter 1 based index

    if(index < 0 || index >= shellHistory.historySize){
        return "";
    }
    if(shellHistory.historySize == 20){
        return shellHistory.inputs[(shellHistory.firstIndex + index) % 20];
    } else {
        return shellHistory.inputs[index];
    }
}

void addHistoryRecord(char input[2048]){
    if(shellHistory.historySize == 20){
        free(shellHistory.inputs[shellHistory.firstIndex]);
        shellHistory.inputs[shellHistory.firstIndex] = strdup(input);
        shellHistory.firstIndex = (shellHistory.firstIndex + 1) % 20;
    } else {
        shellHistory.inputs[shellHistory.historySize] = strdup(input);
        shellHistory.historySize++;
    }
}

void printHistory(){
    for(int i = 0; i < shellHistory.historySize; i++){
        // print 1 based index
        printf("%d %s\n",i+1,getHistoryRecord((shellHistory.firstIndex + i) % 20));
    }
}

// <--- Process Functions --->

void addProcess(process **process_list, cmdLine* cmd, pid_t pid){
    process * newProcess = malloc(sizeof(process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->status = RUNNING;
    newProcess->next = NULL;

    if(*process_list == NULL){
        *process_list = newProcess;
        return;
    }

    process *curr = *process_list;
    // add to end;
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = newProcess;
}

void updateProcessStatus(process* curr, int pid, int status){
    curr->status = status;
}

char* statusToString(int status){
    switch(status){
        case TERMINATED:
            return "TERMINATED";
        case RUNNING:
            return "RUNNING";
        case SUSPENDED:
            return "SUSPENDED";
        default:
            return "UNKNOWN";
    }
}

void updateProcessList(process **process_list){
    process* current = *process_list;
    int status;

    while (current != NULL) {

        pid_t result = waitpid(current->pid, &status, WNOHANG | WSTOPPED | WCONTINUED);

        if (result == -1) {
            if(errno == ECHILD){
                updateProcessStatus(current, current->pid, TERMINATED);
            } else {
                perror("waitpid");
            }
        } else if (result > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                updateProcessStatus(current, current->pid, TERMINATED);
            } else if (WIFSTOPPED(status)) {
                updateProcessStatus(current, current->pid, SUSPENDED);
            } else if (WIFCONTINUED(status)) {
                updateProcessStatus(current, current->pid, RUNNING);
            }
        }
        current = current->next;
    }
}

void removeAllDeadProcesses(process** process_list) {
    process* current = *process_list;
    process* previous = NULL;

    while (current != NULL) {
        // Check if the process has terminated
        if (current->status == TERMINATED) {
            // Update the pointers to remove the process from the list
            if (previous == NULL) {
                // The process to be removed is the first in the list
                *process_list = current->next;
                freeCmdLines(current->cmd);
                free(current);
                current = *process_list;
            } else {
                // The process to be removed is not the first in the list
                previous->next = current->next;
                freeCmdLines(current->cmd);
                free(current);
                current = previous->next;
            }
        } else {
            // Move to the next process in the list
            previous = current;
            current = current->next;
        }
    }
}

void printArray(char const *array[], int size);

void printProcessList(process **process_list){
    updateProcessList(process_list);

    int index = 0;
    process* curr = *process_list;
    //print header
    printf("Index | PID | Status | Command\n");
    while(curr != NULL){
        printf("%d | %d | %s | ",index++,curr->pid, statusToString(curr->status));
        printArray((const char **)curr->cmd->arguments,curr->cmd->argCount);
        curr = curr->next;
    }

    removeAllDeadProcesses(process_list);
}

void freeProcessList(process* process_list){
    process *curr = process_list;
    process *next;
    while(curr != 0){
        freeCmdLines(curr->cmd);
        next = curr->next;
        free(curr);
        curr = next;
    }
}

// <--- Shell Functions --->

void execute(cmdLine *line) {
    if (line->next == 0) {
        int child_pid = fork();
        if (child_pid == -1) {
            perror("fork");
            exit(1);
        }

        if (child_pid == 0) {

            if (line->inputRedirect != NULL) {
                if (freopen(line->inputRedirect, "r", stdin) == NULL) {
                    perror("freopen");
                }
            }

            if (line->outputRedirect != NULL) {
                if (freopen(line->outputRedirect, "w", stdout) == NULL) {
                    perror("freopen");
                }
            }

            execvp(line->arguments[0], line->arguments);
            perror("execvp");
            _exit(1);
        } else {
            if (debugMode) {
                printf("executing command: ");
                printArray((const char **) line->arguments, line->argCount);
                printf("new child pid: %d\n", child_pid);
            }
            addProcess(&processes,line,child_pid);
            int status;
            if (line->blocking) {
                waitpid(child_pid, &status, 0);
            }
        }
    } else{
        if (line->outputRedirect != 0 || line->next->inputRedirect != 0) {
            fprintf(stderr,"piping and redirection are not supported\n");
            return;
        }

        pid_t cpid,cpid2;
        int pipefd[2];

        //create a pipe
        pipe(pipefd);

        cpid = fork();
        if(cpid == -1){
            perror("fork");
            _exit(1);
        }
        if (cpid == 0){

            // <--- child process 1 --->

            close(STDOUT_FILENO);

            // redirect the stdout to the new pipe
            dup2(pipefd[1],STDOUT_FILENO);

            // close the write end of the original pipe because we are using the duplicated one
            close(pipefd[1]);

            // execute the command
            execvp(line->arguments[0], line->arguments);

        } else {
            // <--- parent process --->
            addProcess(&processes,line,cpid);

            close(pipefd[1]);
            cpid2 = fork();
            if(cpid2 == -1){
                perror("fork");
                _exit(1);
            }
            if(cpid2 == 0){
                // <--- child process 2 --->
                // close the write end of the pipe because we are reading
                close(STDIN_FILENO);

                // redirect the input to the new pipe
                dup2(pipefd[0],STDIN_FILENO);

                // close the read end of the original pipe because we are using the duplicated one
                close(pipefd[0]);

                // execute the command
                execvp(line->next->arguments[0], line->next->arguments);
            } else {
                // <--- parent process --->

                addProcess(&processes,line->next,cpid2);
                close(pipefd[0]);
                waitpid(cpid,0,0);
                waitpid(cpid2,0,0);
            }
        }
    }
}

void sendSignal(int pid, int signum) {
    // Send signal to a process
    if (kill(pid, signum) == -1) {
        perror("kill");
    }
}

// <--- Helper Functions --->

void printArray(char const *array[], int size){
    for(int i = 0; i < size; i++){
        printf("%s ",array[i]);
    }
    printf("\n");
}

void readArgs(int argc, char **argv) {
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i],"-d") == 0) {
            debugMode = 1;
        }
    }
}

// <--- Main Function --->

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

        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0 && parsedCmdLine->argCount >= 2) {
            // Change the current working directory
            if (chdir(parsedCmdLine->arguments[1]) == -1) {
                perror("chdir");
            }
            freeCmdLines(parsedCmdLine);
            continue; // Skip the execution step for "cd" command
        }

        if (strcmp(parsedCmdLine->arguments[0], "suspend") == 0 && parsedCmdLine->argCount >= 2) {
            sendSignal(atoi(parsedCmdLine->arguments[1]),SIGTSTP);
            continue;
        }

        if (strcmp(parsedCmdLine->arguments[0], "wakeup") == 0 && parsedCmdLine->argCount >= 2) {
            sendSignal(atoi(parsedCmdLine->arguments[1]),SIGCONT);
            continue;
        }

        if (strcmp(parsedCmdLine->arguments[0], "nuke") == 0 && parsedCmdLine->argCount >= 2) {
            sendSignal(atoi(parsedCmdLine->arguments[1]),SIGINT);
            continue;
        }

        if (strcmp(parsedCmdLine->arguments[0], "procs") == 0) {
            printProcessList(&processes);
            continue;
        }

        // Execute the command
        if (parsedCmdLine != NULL) {
            execute(parsedCmdLine);

            // Free the resources allocated by parseCmdLines
            //freeCmdLines(parsedCmdLine);
        }
    }
    freeProcessList(processes);

    return EXIT_SUCCESS;
}
