#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define CHILD_PATH "CHILD_PATH"
#define CHILD_PATH_KEY_LENGTH 11
#define CHILD_PROGRAM_NAME "child"
#define P_CODE_SLEEP_TIME_SECONDS 5

struct childProcess
{
    pid_t pid;
    int number;
};

char* childName(int childNumber)
{
    char* name = (char*) malloc(32*sizeof(char));
    strcpy(name, "child_");
    
    char p[17];
    snprintf (p, sizeof(p), "%d", childNumber);

    strcat(name, p);
    return name;
}

char* childProgramPath()
{
    char* tmp = (char*) malloc(255*sizeof(char));
    strcpy(tmp, getenv(CHILD_PATH));
    strcat(tmp, CHILD_PROGRAM_NAME);
    return tmp;
}

char** createChildProcessArgv(char* name)
{
    size_t argument_size = 255*sizeof(char);
    char** argv = (char**) malloc(2*sizeof(char*));
    argv[0] = (char*) malloc(argument_size); 
    argv[1] = NULL;
    strcpy(argv[0], name);

    return argv;
}

pid_t createChildProcess(char* programPath, char** argv)
{
    pid_t p = fork();
    
    if(p == 0)
    {
        execve(programPath, argv, NULL);
    }
    else if(p > 0)
    {
        printf("Parent PID = %d\n", getpid());
    }   
    return p;
}

void showProcessInfo(struct childProcess pr)
{
    printf("Name = child_%d, PID = %d\n", pr.number, pr.pid);
}

void processAction(struct childProcess pr, void(*action)(struct childProcess))
{
    action(pr);
}

void allProcessAction(struct childProcess* processesPid, int size, void (*process_action) (struct childProcess))
{
    for(int i = 0; i < size; i++)
        process_action(processesPid[i]);
}

void killProcess(struct childProcess pr)
{
    kill(pr.pid, SIGKILL);
}

void muteProcess(struct childProcess pr)
{
    kill(pr.pid, SIGUSR2);
}

void unmuteProcess(struct childProcess pr)
{
    kill(pr.pid, SIGUSR1);
}

struct childProcess findChild(struct childProcess* list, int size, int number)
{
    for(int i = 0; i < size; i++)
        if(list[i].number == number)
            return list[i];
}

int parseChildNumberFromCommand(char* buff)
{
    int i = 0;
    for(; buff[i] < '0' || buff[i] > '9'; i++)
        ;
    return atoi(buff+i);
}

int main(int argc, char** argv, char** envp)
{
    struct childProcess* processesPid = (struct childProcess*) malloc(255*sizeof(struct childProcess));
    int childProcessesCount = 0;

    char* message[255];

    while(1)
    {
        printf("\n\nInput character:\n");
        rewind(stdin);

        char commandBuff[8];
        scanf("%s", commandBuff);

        if(!strcmp(commandBuff, "+"))
        {
            char* cName = childName(childProcessesCount);
            char** cArgv = createChildProcessArgv(cName);
            char* programPath = childProgramPath();
            pid_t newProcessPid = createChildProcess(programPath, cArgv);
            
            struct childProcess tmp;
            tmp.pid = newProcessPid;
            tmp.number = childProcessesCount;

            processesPid[childProcessesCount++] = tmp;
            printf("\t\t\t\t\t\tNew process was created, pid = %d", newProcessPid);
        }
        else if(!strcmp(commandBuff, "-"))
        {
            struct childProcess lastChildProcess = processesPid[childProcessesCount-1];
            printf("Process with pid = %d was killed\nCount: %d", (int)lastChildProcess.pid , --childProcessesCount);
            processAction(lastChildProcess, killProcess);
        }
        else if(!strcmp(commandBuff, "l"))
            allProcessAction(processesPid, childProcessesCount, showProcessInfo);

        else if(!strcmp(commandBuff, "k") || !strcmp(commandBuff, "q"))
        {
            allProcessAction(processesPid, childProcessesCount, killProcess);
            printf("All processes were removed\n");
            childProcessesCount = 0;
            if(!strcmp(commandBuff, "q")) break;
        }
        else if(!strcmp(commandBuff, "s") || commandBuff[0] == 'p')
        {
            allProcessAction(processesPid, childProcessesCount, muteProcess);
            printf("All processes were muted\n");
            if(commandBuff[0] == 'p')
            {
                struct childProcess child = findChild(processesPid, childProcessesCount, parseChildNumberFromCommand(commandBuff));
                processAction(child, unmuteProcess);
                printf("Child number %d is unmuted\n", child.number);
                
                sleep(P_CODE_SLEEP_TIME_SECONDS);
                allProcessAction(processesPid, childProcessesCount, unmuteProcess);
            }
        }
        else if(commandBuff[0] == 's')
        {
            struct childProcess child = findChild(processesPid, childProcessesCount, parseChildNumberFromCommand(commandBuff));
            processAction(child, muteProcess);
            printf("Child number %d is muted\n", child.number);
        }
        else if(!strcmp(commandBuff, "g"))
        {
            allProcessAction(processesPid, childProcessesCount, unmuteProcess);
            printf("All processes were unmuted\n");
        }
        else if(commandBuff[0] == 'g')
        {
            struct childProcess child = findChild(processesPid, childProcessesCount, parseChildNumberFromCommand(commandBuff));
            processAction(child, unmuteProcess);
            printf("Child number %d is unmuted\n", child.number);
        }
        else if(!strcmp(commandBuff, "cls"))
            system("clear");
    }
    return 0;   
}