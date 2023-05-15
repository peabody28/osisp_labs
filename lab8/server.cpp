#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#define SOCKET_BIND_PORT 8080
#define REQUEST_QUEUE_SIZE 128

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
    bind(listenerSocketDescriptor, address, len);
}

void listenRequests(int listenerSocketDescriptor)
{
    if(listen(listenerSocketDescriptor, REQUEST_QUEUE_SIZE) != 0)
    {
        perror("listen");
        exit(1);
    }
}

/// @return new socket descriptor 
int acceptRequest(int listenerSocketDescriptor)
{
    sockaddr clientSocketAddress; // две переменные, хранящие инфу о адресе клиента...
    socklen_t clientSocketAddressLen; //... вместо ссылок на них можно использовать NULL (так как они не юзаются тут)

    int newSocketDescriptor = accept(listenerSocketDescriptor, &clientSocketAddress, &clientSocketAddressLen);
    
    if(newSocketDescriptor < 0)
    {
        perror("accept");
        exit(3);
    }

    return newSocketDescriptor;
}

void echoRequestHandler(int socketDesc)
{
    char messageBuf[1024];

    while(1)
    {
        ssize_t bytes_read = recv(socketDesc, messageBuf, 1024, 0);
        if(bytes_read <= 0)
            break;
        send(socketDesc, messageBuf, bytes_read, 0);
    }
}

void(*getRequestHandler())(int socketDesc)
{
    return echoRequestHandler;
}

int main()
{
    auto requestHandler = getRequestHandler(); // обработчик запроса задается ссылкой на функцию

    int listenerSocketDescriptor = createListenerSocket();

    sockaddr* listenerSocketAddress = getAddressForBindListenerSocket(INADDR_LOOPBACK, SOCKET_BIND_PORT);

	bindSocketTo(listenerSocketDescriptor, listenerSocketAddress);

    listenRequests(listenerSocketDescriptor);

    while(1)
    {
        int newSocketDescriptor = acceptRequest(listenerSocketDescriptor);

        // поместить в отдельный поток (также в идеале нужно ограничить кол-во создаваемых потоков например до 1к)
        requestHandler(newSocketDescriptor);
    
        close(newSocketDescriptor);
    }

    return 0;
}