#include "datastruct.h"

Message::Message()
{
    
}

void QueueExtension::push(Message m)
{
    messages[inCount-outCount] = m;
    inCount++;
}

Message QueueExtension::pop()
{
    return messages[inCount - outCount++ -1];
}

bool QueueExtension::isEmpty()
{
    return inCount < outCount;
}