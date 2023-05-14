#pragma once
#ifndef node_H
#define node__H
#include <string>
#include "datastruct.h"

enum NodeType
{
    Prod,
    Cons
};

class Node
{
protected:
    int id;
public:
    NodeType type;

    Node(int id, NodeType type);

    int getId();

    virtual bool action(QueueExtension* broker, Message*m) = 0;

    virtual std::string actionMessage(QueueExtension* broker) = 0;
};


#endif