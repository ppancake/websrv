
#ifndef WEBSRV_SOCKET_H
#define WEBSRV_SOCKET_H

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <error.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <cstring>

class Socket
{
public:
    //socket的基本操作
    static int createSocket();
    static void Bind(const int sockfd,const struct sockaddr_in &addr);
    static void Listen(const int sockfd);
    static int Accept(const int sockfd, struct sockaddr_in &addr);
    static void Close(const int sockfd);

    //设置reuse
    static void setReuseAddr(const int fd,bool on);
    //设置非阻塞
    static int setNonblock(int sockfd);

};


#endif //WEBSRV_SOCKET_H
