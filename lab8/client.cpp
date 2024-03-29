#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#define SOCKET_BIND_PORT -1
#define SERVER_PORT 8081

using namespace std;

/// @return socket descriptor 
int createSocket()
{
    int sockerDesc = socket(AF_INET, SOCK_STREAM, 0);
    if(sockerDesc < 0)
    {
        perror("socket");
        exit(1);
    }

    return sockerDesc;
}

sockaddr* getServerSocketAddress(long serverIp, int serverPort)
{
    sockaddr_in* serverSocketAddress = new sockaddr_in;
    serverSocketAddress->sin_family = AF_INET;
    serverSocketAddress->sin_port =  htons(serverPort);
    serverSocketAddress->sin_addr.s_addr = htonl(serverIp);

    return (sockaddr*)serverSocketAddress;
}

int connectToServer(sockaddr* serverSocketAddress)
{
    int socketDesc = createSocket();
    if(connect(socketDesc, serverSocketAddress, sizeof(sockaddr)) != 0)
    {
        perror("connect");
        exit(1);
    }
    cout << "connected to server" << endl;

    return socketDesc;
}

void disconnect(int socketDesc)
{
    close(socketDesc);
}

void sendRequest(sockaddr* serverSocketAddress, const char* data, int len)
{
    int socketDesc = connectToServer(serverSocketAddress);

    send(socketDesc, data, len, 0);
    cout << "message: " << data << " len:" << len << " sended" << endl;

    char buf[len];
    recv(socketDesc, buf, len, 0);
    cout << "message: " << buf << " recieved" << endl;

    disconnect(socketDesc);
}

int main()
{
    sockaddr* serverSocketAddress = getServerSocketAddress(INADDR_LOOPBACK, SERVER_PORT);

    while(true)
    {
        string message;
        cout << "input request (example: \"GET /echo somestring\"): " << endl;
        getline(cin, message);
        
        sendRequest(serverSocketAddress, message.c_str(), message.size());
    }

    return 0;
}