#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <queue>
#include <string>
#include "Thread.cpp"


/*
 * A Simple Server
 * A Callback called for each line read. Bad for time sensitive stuffs.
 */

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