#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>
#include <vector>
#include "consumer.h"
#include "producer.h"
#include <thread>
#include <unistd.h>
#include <mutex>
#include <functional>
using namespace std;

std::mutex g_lock;

struct NodeThread
{
    Node* node;
    thread::id thread_id;
};

void lockAction(const std::function<void()>& f)
{
    try
	{
		g_lock.lock();
		f();
		g_lock.unlock();
	}
	catch (...)
	{
		g_lock.unlock();
	}
}

void listenMessage(Consumer c, QueueExtension* broker)
{
    while(1)
    {
        Message m;
        lockAction([&](){
            if(c.tryReadMessage(broker, &m))
                cout << "Message " << broker->outCount << " readed" << endl; 
        });
        
        sleep(2);
    }
}

void writeMessage(Producer p, QueueExtension* broker)
{
    while(1)
    {
        Message m = p.generateMessage();
        lockAction([&](){
            if(p.tryAddMessage(broker, m))
                cout << "Puts" << broker->inCount << " message" << endl; 
        });
        
        sleep(2);
    }
}


int main(int argc, char** argv, char** envp)
{
    QueueExtension *broker = new QueueExtension();
    broker->inCount = broker->outCount = 0;

    char ch;
    int consumerCount = 0;
    int producerCount = 0;

    vector<NodeThread> nodeThreads;
    while(1)
    {
        cout << endl << endl << "Input character: ";

        cin.clear();
        cin >> ch;
        
        int type;
        if(ch == 'p')
        {
            Producer* p = new Producer(producerCount++);
            thread th(writeMessage, p, broker);
            struct NodeThread nt;
            nt.node = p;
            nt.thread_id = th.get_id();

            nodeThreads.push_back(nt);
        }
        else if(ch == 'c')
        {
            Consumer* c = new Consumer(consumerCount++);
            thread th(listenMessage, c, broker);

            struct NodeThread nt;
            nt.node = c;
            nt.thread_id = th.get_id();
            
            nodeThreads.push_back(nt);
        }
        else
            break;

    }

    return 0;   
}
