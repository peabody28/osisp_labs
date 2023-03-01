#include "consumer.h"
#include "node.h"
#include <string>

Consumer::Consumer(int id) : Node(id, NodeType::Cons) {}

bool Consumer::action(QueueExtension *broker, Message* msg)
{
    if(broker->isEmpty())
        return false;
    *msg = broker->pop();
    return true;
}

std::string Consumer::actionMessage(QueueExtension* broker)
{
    return "Message " + to_string(broker->outCount) + " readed";
}