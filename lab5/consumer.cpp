#include "consumer.h"

Consumer::Consumer(int id) : Node(id) {}

int Consumer::getId()
{
    return id;
}

bool Consumer::tryReadMessage(QueueExtension *broker, Message* msg)
{
    if(broker->isEmpty())
        return false;
    *msg = broker->pop();
    return true;
}