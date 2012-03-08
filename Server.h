#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <queue>
#include <string>
#include "Thread.cpp"

class Server{
public:
    Server(int,bool=true);
    ~Server();
    
    void process(int);
    void setCallback(std::string (*)());
    void start();

private:
    void acceptProc(int);
    bool serverRunning;
    int port,serverSocket;
    std::string (*callback)();
};

#endif //SERVER_H 