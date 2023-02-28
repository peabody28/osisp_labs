#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <queue>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
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

void addMessage(QueueExtension *broker, Message m)
{
    broker->push(m);
}

bool tryReadMessage(QueueExtension *broker, Message* msg)
{
    if(broker->isEmpty())
        return false;
    *msg = broker->pop();
    return true;
}

Message generateMessage()
{
    Message m;
    int num = rand();
    int size = num % 257;
    m.size = size;

    int dataSize = ((size+3)/4)*4;
    strcpy(m.data, "data");

    m.hash = dataSize + m.size;
    return m;
}

int main(int argc, char** argv)
{
    int shmid;
    QueueExtension *broker;
    shmid = shmget(SHM_KEY, sizeof(QueueExtension), perms);
    cout << shmid << endl;
    broker = (QueueExtension*) shmat(shmid, NULL, 0);

    sem_t* sem = sem_open(SEMAPHORE_NAME, O_RDWR);
    
    int type = 1;
    string typeName = type == 1 ? "producer" : "consumer";
    cout << "I am " << argv[0] << " (" << typeName << ") My PID is " << getpid() << endl;

    while(1)
    {
        if(type == 1)
        {
            Message m = generateMessage();
            sem_wait(sem);
            addMessage(broker, m);
            sem_post(sem);
            cout << "PID " << getpid() << " puts message" << endl;
        }
        else
        {
            Message m;
            sem_wait(sem);
            if(tryReadMessage(broker, &m))
                cout << "Message " << broker->outCount << " readed " << "data: " << m.data << endl;
            sem_post(sem);
        }
        sleep(2);
    }

    return 0;
}
