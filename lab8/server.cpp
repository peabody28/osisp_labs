#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <thread>
#include <vector>
#define SOCKET_BIND_PORT 8081
#define REQUEST_QUEUE_SIZE 128
#define MESSAGE_BUFFER_SIZE 1024
using namespace std;

/// @return socket descriptor 
int createListenerSocket()
{
    int sockerDesc = socket(AF_INET, SOCK_STREAM, 0);
    if(sockerDesc < 0)
    {
        perror("socket");
        exit(1);
    }
    cout << "socket created" << endl;

    return sockerDesc;
}

sockaddr* getAddressForBindListenerSocket(long ip, int port)
{
    sockaddr_in* addr = new sockaddr_in;
    addr->sin_family = AF_INET;
    addr->sin_port =  htons(port);
    addr->sin_addr.s_addr = htonl(ip);

    return (sockaddr*)addr;
}

void bindSocketTo(int listenerSocketDescriptor, sockaddr *address)
{
    int len = sizeof(sockaddr);
    if(bind(listenerSocketDescriptor, address, len) != 0)
    {
        perror("bind");
        exit(1);
    }
    cout << "socket binded" << endl;
}

void listenRequests(int listenerSocketDescriptor)
{
    if(listen(listenerSocketDescriptor, REQUEST_QUEUE_SIZE) != 0)
    {
        perror("listen");
        exit(1);
    }

    cout << "start listen..." << endl;
}

/// @return new socket descriptor 
int acceptRequest(int listenerSocketDescriptor)
{
    int newSocketDescriptor = accept(listenerSocketDescriptor, NULL, NULL);

    if(newSocketDescriptor < 0)
    {
        perror("accept");
        exit(3);
    }

    cout << "request from accepted";

    return newSocketDescriptor;
}

string getRequestRoute(char* buffer)
{
    // Получение пути запроса
    std::string requestPath;
    char* pathStart = strchr(buffer, ' ') + 1;  // Начало пути после первого пробела
    char* pathEnd = strchr(pathStart, ' ');  // Конец пути перед вторым пробелом
    if (pathStart != nullptr && pathEnd != nullptr)
        requestPath.assign(pathStart, pathEnd - pathStart);

    return requestPath;
}

string getData(const char* buffer)
{
    char* bufferCopy = new char[1024];
    strcpy(bufferCopy, buffer);

    std::string requestPath;
    char* secondSpace = strchr(strchr(bufferCopy, ' ') + 1, ' ');

    return string(secondSpace);
}

void echoRequestHandler(int socketDesc, string data)
{
    send(socketDesc, data.c_str(), data.size()+1, 0);
}

void reverseRequestHandler(int socketDesc, string data)
{
    string dataCopy(data);

    reverse(dataCopy.begin(), dataCopy.end());

    send(socketDesc, dataCopy.c_str(), dataCopy.size()+1, 0);
}

void(*getRequestHandler(string path))(int sockDesc, string data)
{
    if(!path.compare("/echo"))
        return echoRequestHandler;
    else if(!path.compare("/reverse"))
        return reverseRequestHandler;
    
    perror("endpoint not exists");
    exit(1);
}

void handleRequest(int socketDesc)
{
    char* messageBuf = new char[MESSAGE_BUFFER_SIZE];

    recv(socketDesc, messageBuf, MESSAGE_BUFFER_SIZE, 0);

    string path = getRequestRoute(messageBuf);
    string data = getData(messageBuf);

    auto requestHandler = getRequestHandler(path);

    requestHandler(socketDesc, data);
    close(socketDesc);
   
    free(messageBuf);
}

int main()
{
    int listenerSocketDescriptor = createListenerSocket();
    sockaddr* listenerSocketAddress = getAddressForBindListenerSocket(INADDR_LOOPBACK, SOCKET_BIND_PORT);
	bindSocketTo(listenerSocketDescriptor, listenerSocketAddress);
    listenRequests(listenerSocketDescriptor);

    std::vector<std::thread> threads;
    while(1)
    {
        int newSocketDescriptor = acceptRequest(listenerSocketDescriptor);

        threads.emplace_back(handleRequest, newSocketDescriptor);
        threads.back().detach();
    }

    close(listenerSocketDescriptor);

    return 0;
}