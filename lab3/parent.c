#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define CHILD_PATH "CHILD_PATH"
#define CHILD_PATH_KEY_LENGTH 11
#define CHILD_PROGRAM_NAME "child"

void (*process_action) (pid_t);

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
        wait(NULL);
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

pid_t lastProcess(pid_t* processesPid, int size)
{
    return processesPid[size-1];
}

void showProcessInfo(pid_t pid)
{
    printf("pid = %d", pid);
}

void allProcessAction(pid_t* processesPid, int size, process_action* action)
{
    for(int i = 0; i < size; i++)
        action(processesPid[i]);
}

void killProcess(pid_t pid)
{
    kill(pid, 9);
}


int main(int argc, char** argv, char** envp)
{
    int childProcessesCount = 0;

    char ch;
    pid_t* processesPid = (pid_t*) malloc(255*sizeof(pid_t));
    while(1)
    {
        printf("\n\ninput character: ");

        rewind(stdin);

        ch = getchar(); getchar();
        
        if(ch == '+')
        {
            char* cName = childName(childProcessesCount);
            char** cArgv = createChildProcessArgv(cName);
            char* programPath = childProgramPath();
            pid_t newProcessPid = createChildProcess(programPath, cArgv, NULL);
            processesPid[childProcessesCount++] = newProcessPid;
            printf("New process was created, pid = %d", newProcessPid);
        }
        else if(ch == '-')
        {
            pid_t last_process = lastProcess(processesPid, childProcessesCount--);
            kill(lastProcess, 9);
            printf("Process with pid = %d was killed\nCount: ", lastProcess, childProcessesCount);
        }
        else if(ch == 'l')
            allProcessAction(processesPid, childProcessesCount, showProcessInfo);
        else if(ch == 'k' || ch == 'q')
        {
            allProcessAction(processesPid, childProcessesCount, killProcess);
            printf("All processes were removed\n");
            
            if(ch == 'q')
                break;
        }
            
    }
    return 0;   
}