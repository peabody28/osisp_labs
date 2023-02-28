#include <iostream>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <vector>
#include <sys/shm.h>
#include "datastruct.h"
#define CHILD_PATH "./"
#define CHILD_PROGRAM_NAME "child"
#define perms 0777
#define SHM_KEY 0x1234
#define SEMAPHORE_NAME "/semName"

using namespace std;

string childName(int childNumber)
{
    return CHILD_PROGRAM_NAME + to_string(childNumber);
}

pid_t createChildProcess(char* programPath, char** argv, char** envp)
{
    pid_t p = fork();
    
    if(p == 0)
    {
        execve(programPath, argv, envp);
    }
    else if(p > 0)
    {
        cout << "Parent PID = " << getpid() << endl;
    }   
    return p;
}

char* childProgramPath()
{
    char* tmp = new char[255];
    strcpy(tmp, CHILD_PATH);
    strcat(tmp, CHILD_PROGRAM_NAME);
    return tmp;
}

char** createChildProcessArgv(const char* name, int type)
{
    size_t argument_size = 255*sizeof(char);
    char** argv = new char*[3];
    argv[0] = new char[argument_size]; 
    argv[1] = new char[argument_size]; 
    argv[2] = NULL;
    strcpy(argv[0], name);
    strcpy(argv[1], to_string(type).c_str());

    return argv;
}

int main(int argc, char** argv, char** envp)
{
    int shmid = shmget(SHM_KEY, sizeof(QueueExtension), IPC_CREAT | perms);
    QueueExtension *broker = (QueueExtension*) shmat(shmid, NULL, 0);
    broker->inCount = broker->outCount = 0;

    sem_t* sem = sem_open(SEMAPHORE_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR, 1);

    int childProcessesCount = 0;

    vector<pid_t> processes;
    char ch;
    while(1)
    {
        cout << endl << endl << "Input character: ";

        cin.clear();
        cin >> ch;
        
        int type;
        if(ch == 'p') type = 1;
        else if(ch == 'c') type = 0;
        else break;

        string cName = childName(childProcessesCount++);
        char** cArgv = createChildProcessArgv(cName.c_str(), type);
        char* programPath = childProgramPath();

        pid_t childPid = createChildProcess(programPath, cArgv, envp);
        processes.push_back(childPid);
    }

    for(auto pid : processes) kill(pid, SIGKILL);

    sem_close(sem);
    sem_destroy(sem);
    close(shmid);

    return 0;   
}
