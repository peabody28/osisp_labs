 #include "producer.h"

 Producer::Producer(int id) : Node(id) {}

int Producer::getId()
{
    return id;
}

bool Producer::tryAddMessage(QueueExtension *broker, Message m)
{
    if(broker->inCount >= 255)
        return false;
    broker->push(m);
    return true;
}

char* Producer::randstring(int length) 
{

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

Message Producer::generateMessage()
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