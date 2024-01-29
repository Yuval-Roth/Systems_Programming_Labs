#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){
    int pipefd[2];
    pid_t cpid;
    char *buf = 0;

    int status;

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(2);
    }
    if((cpid = fork()) == -1){
        perror("fork");
        exit(2);
    }

    if(cpid == 0){
        close(pipefd[0]);
        if (write(pipefd[1],"hello",5) == -1){
            perror("write");
        }
        close(pipefd[1]);
        exit(1);
    } else {
        buf = (char*)malloc(5);
        waitpid(cpid,&status,0);
        close(pipefd[1]);
        if(read(pipefd[0],buf,5) == -1){
            perror("read");
        }
        if(write(STDOUT_FILENO,buf,5) == -1){
            perror("write");
        }
        close(pipefd[0]);
    }







}
