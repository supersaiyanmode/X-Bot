#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cctype>
#include "Client.h"

namespace {
#ifdef WIN32
    int read_(int sock, char* arr, int len){
        return recv(sock, arr,len,0);
    }
    int write_(int sock, const char* arr, int len){
        return send(sock,arr, len, 0);
    }
    int close_(int sock){
        return closesocket(sock);
    }
    WSADATA wsaData;
    int dummy = WSAStartup(MAKEWORD(2,2), &wsaData);
#define socklen_t int
#else
    int read_(int sock, char* arr, int len){
        return ::read(sock, arr,len);
    }
    int write_(int sock, const char* arr, int len){
        return ::write(sock,arr, len);
    }
    int close_(int sock){
        return ::close(sock);
    }
#endif 
    void error(const char *msg){
        perror(msg);
        //exit(1);
    }

    bool readLine(int socket, std::string &ret){
        if (socket == -1)
            return false;
        ret="";
        while (1){
            char cur;
            //std::cout<<"Read till now: "<<ret<<std::endl;
            if (read_(socket,&cur,1)<=0){
                return false;
            }
            if (cur == '\n'){
                return true;
            }
            ret.append(1,cur); //not great, quick and dirty.
        }
    }

    bool writeLine(int socket, const std::string& line){
        if (socket == -1)
            return false;
        std::string str(line);
        int len = line.length();
        if (str[len-1] != '\n')
            str.append(1,'\n');
        return write_(socket,str.c_str(), str.length()) >= 0;
    }
}

//###########CLIENT########################
Client::Client(){
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket<0){
        error("ERROR creating the socket.");
        return;
    }
    std::memset((char*)&serv_addr, 0, sizeof(serv_addr));
}
int Client::connect(std::string address, int p){
    port = p;
    serverAddress = address;
    
    server = gethostbyname(serverAddress.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    std::memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if (::connect(clientSocket,(sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR connecting");
        return 0;
    }
    return 1;
}

std::string Client::requestLine(const std::string& s){
    //WARNING: No reader/writer thread should NOT be active.
    ::writeLine(clientSocket,s);
    std::string str;
    ::readLine(clientSocket,str);
    return str;
}

Client::~Client(){
    ::close_(clientSocket);
}