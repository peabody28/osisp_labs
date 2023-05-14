#pragma once
#ifndef consumer_H
#define consumer_H

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

    bool action(QueueExtension *broker, Message* msg) override;

    string actionMessage(QueueExtension* broker) override;
};

#endif
