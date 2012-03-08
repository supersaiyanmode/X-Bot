#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <queue>
#include <string>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include "Thread.cpp"

class Client{
public:
    Client();
    ~Client();
    
    std::string requestLine(const std::string&);
    int connect(std::string, int);
protected:
    int port;
    std::string serverAddress;
    int clientSocket;
    sockaddr_in serv_addr;
    hostent *server;
};

#endif //SERVER_H 