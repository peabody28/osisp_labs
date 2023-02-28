#pragma once
class Message
{
public:
    char type;
    short hash; // hashsum
    char size;
    char data[255];
};

class QueueExtension
{
public:
    Message messages[255];
    int inCount = 0;
    int outCount = 0;
    
    void push(Message m);

    Message pop();

    bool isEmpty();
};
