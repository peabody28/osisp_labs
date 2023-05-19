#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>
#include <vector>
#include "node.h"
#include "consumer.h"
#include "producer.h"
#include <thread>
#include <unistd.h>
#include <mutex>
#include <functional>
#include <condition_variable>
using namespace std;

std::mutex g_lock;
std::condition_variable cv;
bool isBusy = false;

void lockAction(const std::function<void()>& f)
{
    std::unique_lock<std::mutex> lk(g_lock);

    try
	{
        cv.wait(lk, []{ return !isBusy;});
        isBusy = true;

		f();
		
        isBusy = false;
        lk.unlock();
        cv.notify_one();
	}
	catch (...)
	{
		isBusy = false;
        lk.unlock();
        cv.notify_one();
	}
}

void action(Node* node, QueueExtension* broker, bool* cancellationToken)
{
    while(!(*cancellationToken))
    {
        Message* msg = node->type == NodeType::Prod ? Producer::generateMessage() : new Message();
        lockAction([&](){
            if(node->action(broker, msg))
                cout << node->actionMessage(broker) << endl; 
        });
        free(msg);
        sleep(2);
    }
    cout << "Thread exited" << endl;
}

Node* nodeResolver(char type, int producersCount, int consumersCount)
{
    Node* node = type == 'p' ? (Node*)new Producer(producersCount) : (Node*)new Consumer(consumersCount);
    return node;
}

int main(int argc, char** argv, char** envp)
{
    QueueExtension *broker = new QueueExtension();
    broker->inCount = broker->outCount = 0;

    char ch;
    int consumerCount = 0;
    int producerCount = 0;

    vector<thread> threads;
    vector<bool*> cancellationTokens;
    while(1)
    {
        cout << endl << endl << "Input character: "  << endl;

        cin.clear();
        cin >> ch;
        
        if(ch == 'k')
        {
            if(!threads.size())
                cout << "No one thread here" << endl;
            else
            {
                *cancellationTokens.back() = true;
                cancellationTokens.pop_back();
                threads.back().join();
                threads.pop_back();
            }
        }
        else
        {
            Node* node = nodeResolver(ch, producerCount, consumerCount);

            bool* cancellationToken = new bool;
            cancellationTokens.push_back(cancellationToken);
            threads.emplace_back(action, node, broker, cancellationToken);
        }
    }

    return 0;   
}
