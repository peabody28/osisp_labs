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
