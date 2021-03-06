#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cctype>
#include <map>

#include "Server.h"
#include "SocketUtils.h"

const std::string CRLF = "\r\n";

void error(const char *msg){
    ::perror(msg);
    //exit(1);
}

Server::Server(int p,bool forceUse):port(p),callback(0){
    //Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    if (serverSocket < 0) 
        error("ERROR opening socket");
    std::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    int optval = forceUse?1:0;
    
    //Allow re-use of socket. 
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof optval);
    
    //Bind it!
    if (bind(serverSocket, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(serverSocket,10);
}

void Server::start(){
    //staer the accept thread.
    Thread<Server,int>(*this, &Server::acceptProc,0).start();
}

void Server::acceptProc(int){
    serverRunning = true;
    std::cout<<"Server: Listening on port "<<port<<std::endl;
    while (serverRunning){
        sockaddr_in client;
        socklen_t clilen = sizeof(client);
        //std::cout<<"Waiting for connection\n";
        int clientSocket = ::accept(serverSocket, (sockaddr*) &client, &clilen);
        if (clientSocket < 0){
            error("ERROR on accept");
        }else{
            //We got a connection. Start a thread for it.
            Thread<Server,int>(*this, &Server::process,clientSocket).start();
        }
    }
}

//Process. Thread lives till socket is open.
void Server::process(int socket){
    std::string line;
    while (1){
        if (!::readLine(socket, line)){ //glCheckers program has been closed..
            ::closeSocket(socket);
            return;
        }
        if (line == "GET" && callback)
            ::writeLine(socket,(*callback)());
    }
}

//Callback
void Server::setCallback(std::string (*fn)()){
    callback = fn;
}

Server::~Server(){
    ::closeSocket(serverSocket);
    std::cout<<"Server shut down.\n";
}
