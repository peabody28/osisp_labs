#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define CHILD_PATH "CHILD_PATH"
#define CHILD_PATH_KEY_LENGTH 11
#define CHILD_PROGRAM_NAME "child"

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

void showEnvironmentVariables(char** envp)
{
    for(int i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }
}

void createChildProcess(char* programPath, char** argv, char** envp)
{
    pid_t p = fork();
    
    if(p == 0)
    {
        execve(programPath, argv, envp);
    }
    else if(p > 0)
    {
        printf("Parent PID = %d\n", getpid());
        wait(NULL);
    }   
}

char* getValue(const char* key, int keySize, char** envp)
{
    for(int i = 0; envp[i] != NULL; i++) 
    {
        char* tmp = (char*) malloc(keySize*sizeof(char));
        strcpy(tmp, envp[i]);
        tmp[keySize-1]='\0';

        if(!strcmp(CHILD_PATH, tmp))
        {
            char* value = (char*) malloc(255*sizeof(char));
            strcpy(value, envp[i]+CHILD_PATH_KEY_LENGTH);
            return value;
        }  
    }
}

char* childProgramPath(int flag, char** envp)
{
    if(flag == 1)
    {
        char* tmp = (char*) malloc(255*sizeof(char));
        strcpy(tmp, getenv(CHILD_PATH));
        strcat(tmp, CHILD_PROGRAM_NAME);
        return tmp;
    }

    if(flag == 2 && envp != NULL)
        return strcat(getValue(CHILD_PATH, CHILD_PATH_KEY_LENGTH, envp), CHILD_PROGRAM_NAME);
    
    if(flag == 3)
    {
        extern char** environ;
        return strcat(getValue(CHILD_PATH, CHILD_PATH_KEY_LENGTH, environ), CHILD_PROGRAM_NAME);
    }

    return NULL;
}

char** createChildProcessArgv(char* name, char* filename)
{
    size_t argument_size = 255*sizeof(char);
    char** argv = (char**) malloc(3*sizeof(char*));
    argv[0] = (char*) malloc(argument_size); 
    argv[1] = (char*) malloc(argument_size); 
    argv[2] = NULL;
    strcpy(argv[0], name);
    strcpy(argv[1], filename);

    return argv;
}

int main(int argc, char** argv, char** envp)
{
    int childProcessesCount = 0;
    char* filename = (char*) malloc(255*sizeof(char));
    strcpy(filename, argv[1]);

    showEnvironmentVariables(envp);

    char ch;

    while(1)
    {
        printf("\n\ninput character: ");

        rewind(stdin);

        ch = getchar(); getchar();
        
        int flag;
        switch (ch)
        {
            case '&': flag = 3; break;
            case '*': flag = 2; break;
            case '+':
            default : flag = 1; break;
        }

        char* cName = childName(childProcessesCount++);
        char** cArgv = createChildProcessArgv(cName, filename);
        char* programPath = childProgramPath(flag, envp);
        createChildProcess(programPath, cArgv, envp);
    }
    return 0;   
}