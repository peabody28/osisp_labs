#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <functional>
#include "datastruct.h"
#define perms 0777
#define SHM_KEY 0x1234
#define SEMAPHORE_NAME "/semName"

using namespace std;

bool tryAddMessage(QueueExtension *broker, Message m)
{
    if(broker->inCount >= 255)
        return false;
    broker->push(m);
    return true;
}

bool tryReadMessage(QueueExtension *broker, Message* msg)
{
    if(broker->isEmpty())
        return false;
    *msg = broker->pop();
    return true;
}

char* randstring(int length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";        
    char *randomString = NULL;

    if (length) 
    {
        randomString = new char[length+1];

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

Message generateMessage()
{
    Message m;
    int num = rand();
    int size = num % 257;
    m.size = size;
    int dataSize = ((size+3)/4)*4;    

    strcpy(m.data, randstring(dataSize));

    m.hash = dataSize + m.size;
    return m;
}

void lockAction(sem_t * sem, const function<void()>& f)
{
	try
	{
		sem_wait(sem);
		f();
		sem_post(sem);
	}
	catch (...)
	{
		sem_post(sem);
	}
}

int main(int argc, char** argv)
{
    int shmid = shmget(SHM_KEY, sizeof(QueueExtension), perms);
    QueueExtension *broker = (QueueExtension*) shmat(shmid, NULL, 0);

    sem_t* sem = sem_open(SEMAPHORE_NAME, O_RDWR);
    
    int type = atoi(argv[1]);
    string typeName = type == 1 ? "producer" : "consumer";
    cout << "I am " << argv[0] << " (" << typeName << ") My PID is " << getpid() << endl;

    while(1)
    {
        if(type == 1)
        {
            Message m = generateMessage();
            lockAction(sem, [&]() {
                if(tryAddMessage(broker, m))
                    cout << argv[0] << "(PID " << getpid() << ") puts message" << endl; 
            });
        }
        else
        {
            Message m;
            lockAction(sem, [&]() {
                if(tryReadMessage(broker, &m))
                    cout << "Message " << broker->outCount << " readed (" << argv[0] << ") data: " << m.data << endl; 
            });
        }
        sleep(2);
    }

    return 0;
}
