#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H
#include <string>

#ifdef WIN32
#else
#endif

#ifdef WIN32
    #include <windows.h>
    #include <winsock2.h>
    #define socklen_t int
#else
    #include <unistd.h>
    #include <sys/types.h> 
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif 

/*
 * SOCKET UTILS
 * Made by best effort to keep it cross platform, providing a higher layer.
 * Didnt much test under windows.
 * 
 */

bool readLine(int socket, std::string &ret);

bool readBytes(int socket, char *buf, int len);

bool writeString(int socket, const std::string& line);

bool writeLine(int socket, const std::string& str);

bool writeBytes(int socket, char *buf, int len);

void closeSocket(int socket);
#endif