#pragma once
#ifndef datastruct_H
#define datastruct_H

class Message
{
public:
    char type;
    short hash; // hashsum
    char size;
    char data[255];

    Message();
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

#endif