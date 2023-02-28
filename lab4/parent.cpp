#include <iostream>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <queue>
#include <sys/shm.h>
#define CHILD_PATH "./"
#define CHILD_PROGRAM_NAME "child"
#define shmsz 20
#define perms 0777
#define SHM_KEY 0x1234
#define SEMAPHORE_NAME "/semName"

using namespace std;

class Message
{
public:
    char type;
    short hash; // hashsum
    char size;
    char data[255];

    Message(){}
};

class QueueExtension
{
public:
    Message messages[255];
    int inCount = 0;
    int outCount = 0;
    void push(Message m)
    {
        messages[inCount-outCount] = m;
        inCount++;
    }

    Message pop()
    {
        return messages[inCount - outCount++ -1];
    }

    bool isEmpty()
    {
        return inCount == outCount;
    }
};

string childName(int childNumber)
{
    return CHILD_PROGRAM_NAME + to_string(childNumber);
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
        cout << "Parent PID = " << getpid() << endl;
    }   
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
    int shmid;
    QueueExtension *broker;
    shmid = shmget(SHM_KEY, sizeof(QueueExtension), IPC_CREAT | perms);
    cout << shmid << endl;
    broker = (QueueExtension*) shmat(shmid, NULL, 0);
    if(!broker)
        return 1;
    broker->inCount = broker->outCount = 0;

    sem_t* sem = sem_open(SEMAPHORE_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR, 1);

    int childProcessesCount = 0;

    char ch;
    while(1)
    {
        cout << endl << endl << "Input character: ";

        cin.clear();
        cin >> ch;
        
        int type = ch == 'p' ? 1 : 0;

        string cName = childName(childProcessesCount++);
        char** cArgv = createChildProcessArgv(cName.c_str(), type);
        char* programPath = childProgramPath();

        createChildProcess(programPath, cArgv, envp);
    }

    return 0;   
}
