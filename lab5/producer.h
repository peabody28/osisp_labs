#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include "datastruct.h"
#include "node.h"
using namespace std;

class Producer: public Node
{
public:
    Producer(int id);

    int getId();

    bool tryAddMessage(QueueExtension *broker, Message m);

    char* randstring(int length);

    Message generateMessage();
};
