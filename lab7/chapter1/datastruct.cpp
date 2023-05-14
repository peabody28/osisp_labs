#include "datastruct.h"

Message::Message()
{
    
}

void QueueExtension::push(Message m)
{
    messages.push(m);
    inCount++;
}

Message QueueExtension::pop()
{
    Message m = messages.front();
    messages.pop();
    outCount++;
    return m;
}

bool QueueExtension::isEmpty()
{
    return messages.empty();
}