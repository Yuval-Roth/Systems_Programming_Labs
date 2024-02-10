#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int sig)
{
    printf("\nRecieved Signal : %s\n", strsignal(sig));
    if (sig == SIGTSTP)
    {
        signal(SIGTSTP, SIG_DFL);
    }
    else if (sig == SIGCONT)
    {
        signal(SIGCONT, SIG_DFL);
    }
    signal(sig, SIG_DFL);
    raise(sig);
}

int main(int argc, char **argv)
{

    printf("Starting the program\n");
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCONT, handler);

    __pid_t i = getpid();
    printf("looper pid: %d\n", i);

    while (1)
    {
        sleep(10);
        printf("looper is running\n");
    }

    return 0;
}