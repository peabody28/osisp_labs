 #include "producer.h"

Producer::Producer(int id) : Node(id, NodeType::Prod) {}

bool Producer::action(QueueExtension *broker, Message* m)
{
    broker->push(*m);
    return true;
}

std::string Producer::actionMessage(QueueExtension *broker)
{
    return "Puts " + to_string(broker->inCount) + " message";
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

Message* Producer::generateMessage()
{
    Message* m = new Message();
    int num = rand();
    int size = num % 257;
    m->size = size;
    int dataSize;
    
    while(!(dataSize = ((size+3)/4)*4))
        ;    

    strcpy(m->data, randstring(dataSize));

    m->hash = dataSize + m->size;
    return m;
}