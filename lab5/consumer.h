#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include "datastruct.h"
#include "node.h"
using namespace std;

class Consumer : public Node
{
public:

    Consumer(int id);

    int getId();

    bool tryReadMessage(QueueExtension *broker, Message* msg);
};
