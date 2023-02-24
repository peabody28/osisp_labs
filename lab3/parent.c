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

struct childProcess
{
    pid_t pid;
    int number;
};

char* childName(int childNumber)
{
    char* name = (char*) malloc(32*sizeof(char));
    strcpy(name, "child_");
    
    char* num = (char*) malloc(2*sizeof(char));
    num[0] = childNumber+'0';
    num[1] = '\0';
    
    strcat(name, num);
    return name;
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

void showProcessInfo(struct childProcess pr)
{
    printf("pid = %d\n", pr.pid);
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

int main(int argc, char** argv, char** envp)
{
    int childProcessesCount = 0;

    char ch;
    struct childProcess* processesPid = (struct childProcess*) malloc(255*sizeof(struct childProcess));
    while(1)
    {
        printf("\n\nInput character:\n");

        rewind(stdin);

        char buff[3]; buff[2] = '\0';
        scanf("%s", buff);
        if(!strcmp(buff, "+"))
        {
            char* cName = childName(childProcessesCount);
            char** cArgv = createChildProcessArgv(cName);
            char* programPath = childProgramPath();
            pid_t newProcessPid = createChildProcess(programPath, cArgv);
            
            struct childProcess tmp;
            tmp.pid = newProcessPid;
            tmp.number = childProcessesCount;

            processesPid[childProcessesCount++] = tmp;
            printf("New process was created, pid = %d", newProcessPid);
        }
        else if(!strcmp(buff, "-"))
        {
            struct childProcess lastChildProcess = processesPid[childProcessesCount-1];
            killProcess(lastChildProcess);
            printf("Process with pid = %d was killed\nCount: %d", (int)lastChildProcess.pid , --childProcessesCount);
        }
        else if(!strcmp(buff, "l"))
            allProcessAction(processesPid, childProcessesCount, showProcessInfo);

        else if(!strcmp(buff, "k") || !strcmp(buff, "q"))
        {
            allProcessAction(processesPid, childProcessesCount, killProcess);
            printf("All processes were removed\n");
            childProcessesCount=0;
            if(!strcmp(buff, "q")) break;
        }
        else if(!strcmp(buff, "s") || buff[0] == 'p')
        {
            allProcessAction(processesPid, childProcessesCount, muteProcess);
            printf("All processes were muted\n");
            if(buff[0] == 'p')
            {
                struct childProcess child = findChild(processesPid, childProcessesCount, buff[1]-'0');
                unmuteProcess(child);
                printf("Child number %d is unmuted\n", child.number);
            }
        }
        else if(buff[0] == 's')
        {
            struct childProcess child = findChild(processesPid, childProcessesCount, buff[1]-'0');
            muteProcess(child);
            printf("Child number %d is muted\n", child.number);
        }
        else if(!strcmp(buff, "g"))
        {
            allProcessAction(processesPid, childProcessesCount, unmuteProcess);
            printf("All processes were unmuted\n");
        }
        else if(buff[0] == 'g')
        {
            struct childProcess child = findChild(processesPid, childProcessesCount, buff[1]-'0');
            unmuteProcess(child);
            printf("Child number %d is unmuted\n", child.number);
        }
    }
    return 0;   
}