#pragma once
#ifndef producer_H
#define producer_H
#include <iostream>
#include <string>
#include <string.h>
#include "datastruct.h"
#include "node.h"

using namespace std;

class Producer: public Node
{
private:

    static char* randstring(int length);

public:
    Producer(int id);

    int getId();

    bool action(QueueExtension *broker, Message* m) override;

    static Message* generateMessage();

    std::string actionMessage(QueueExtension* broker) override;
};

#endif
