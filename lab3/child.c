#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define _POSIX_SOURCE
#include <signal.h>

int isPrintingAllowed = 1;

void isPrintingAvailable(int sig)
{
    isPrintingAllowed = sig == SIGUSR1 ? 1 : 0;
}

int main(int argc, char** argv)
{
    struct sigaction allowPrintAction, denyPrintAction;
    allowPrintAction.sa_handler = denyPrintAction.sa_handler = isPrintingAvailable;
    allowPrintAction.sa_flags = denyPrintAction.sa_flags = SA_RESTART;

    sigaction (SIGUSR1, &allowPrintAction, NULL);
    sigaction (SIGUSR2, &denyPrintAction, NULL);

    pid_t pid = getpid();
    while(1)
    {
        if(isPrintingAllowed == 1)
            printf("i'm %s, my PID=%d \n\n", argv[0], pid);
        sleep(2);
    }
    
    return 0;
}