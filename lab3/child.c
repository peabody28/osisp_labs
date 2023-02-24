#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#define _POSIX_SOURCE
#include <signal.h>
#define REPEAT_COUNT 151000000

enum StatVariable
{
    First,
    Second
};

struct data // {0, 0} || {1, 1}
{
    int first;
    int second;
};


int isAlarmCompleted = 1;

int isPrintingAllowed = 1;

void isPrintingAvailable(int sig)
{
    isPrintingAllowed = sig == SIGUSR1 ? 1 : 0;
}

void alarmHandler()
{
    isAlarmCompleted = 1;
}

void updateStat(struct data* statistics, enum StatVariable* statVariable)
{
    if(*statVariable == First)
        statistics->first = !statistics->first;
    else
        statistics->second = !statistics->second;

    *statVariable = !(*statVariable);
}

int main(int argc, char** argv)
{
    struct sigaction allowPrintAction, denyPrintAction;
    allowPrintAction.sa_handler = denyPrintAction.sa_handler = isPrintingAvailable;
    allowPrintAction.sa_flags = denyPrintAction.sa_flags = SA_RESTART;

    sigaction (SIGUSR1, &allowPrintAction, NULL);
    sigaction (SIGUSR2, &denyPrintAction, NULL);

    signal (SIGALRM, alarmHandler);

    struct data statistics;

    pid_t pid = getpid();
    int iterationsCount = 0;

    enum StatVariable statVariable = First; 
    int differrentStatVariablesCount = 0;
    while(1)
    {
        updateStat(&statistics, &statVariable);

        if(isAlarmCompleted == 1)
        {
            isAlarmCompleted = 0;
            if(statistics.first != statistics.second)
                differrentStatVariablesCount++;

            struct timeval timerVal; timerVal.tv_usec = 200;
            struct itimerval timer; timer.it_value = timerVal; 
            setitimer(ITIMER_REAL, &timer, NULL);
        }

        if(isPrintingAllowed == 1 && iterationsCount++ >= REPEAT_COUNT)
        {
            printf("I am %s. My PID is %d. Stat = %d\n", argv[0], pid, differrentStatVariablesCount);
            differrentStatVariablesCount = 0;
            iterationsCount = 0;
        } 
    }
    
    return 0;
}