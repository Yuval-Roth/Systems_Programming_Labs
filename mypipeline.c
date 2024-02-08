


#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char ** argv){

    pid_t cpid,cpid2;
    int pipefd[2];

    //create a pipe
    pipe(pipefd);

    printf("(parent_process>forking...)\n");
    cpid = fork();
    if(cpid == -1){
        perror("fork");
        _exit(1);
    }
    if (cpid == 0){
        
        // <--- child process 1 --->
        char *argv_cp1[] = {"-ls -l",NULL};

        printf("(child1>redirecting stdout to the write end of the pipe...)\n");
        printf("(child1>going to execute cmd: ls -l)\n");
        // redirect the stdout to the new pipe
        close(STDOUT_FILENO);
        dup2(pipefd[1],STDOUT_FILENO);

        // close the write end of the original pipe because we are using the duplicated one
        close(pipefd[1]);

        // execute the command
        execvp("ls", argv_cp1);

    } else {
        
        // <--- parent process --->
        
        printf("(parent_process>created process with id: %d\n",cpid);
        printf("(parent_process>closing the write end of the pipe...)\n");
        close(pipefd[1]);
        cpid2 = fork();
        if(cpid2 == -1){
            perror("fork");
            _exit(1);
        }
        if(cpid2 == 0){
            
            // <--- child process 2 --->

            char* argv_cp2[] = {"tail","-n","2",NULL};

            // close the write end of the pipe because we are reading
            close(STDIN_FILENO);

            // redirect the input to the new pipe
            printf("(child2>redirecting stdin to the read end of the pipe...)\n");
            dup2(pipefd[0],STDIN_FILENO);

            // close the read end of the original pipe because we are using the duplicated one
            close(pipefd[0]);

            // execute the command
            printf("(child2>going to execute cmd: tail -n 2)\n");
            execvp("tail",argv_cp2);

        } else {
            
            // <--- parent process --->
            
            printf("(parent_process>closing the read end of the pipe...)\n");
            close(pipefd[0]);

            printf("(parent_process>waiting for child processes to terminate...)\n");
            waitpid(cpid,0,0);
            waitpid(cpid2,0,0);
            printf("(parent_process>exiting...)\n");
        }
    }
}