
#include "Socket.h"

int Socket::Accept(const int sockfd, struct sockaddr_in &addr)
{
    socklen_t len=sizeof(addr);
    //多了第四个参数
    int connfd=accept4(sockfd,(struct sockaddr*)&addr,&len,SOCK_NONBLOCK|SOCK_CLOEXEC);

    if(connfd<0)
    {
        std::cout<<"Socket:accept error"<<strerror(errno)<<std::endl;
        exit(1);
    }
    return connfd;
}


void Socket::Bind(const int sockfd, const struct sockaddr_in &addr)
{
    int ret=bind(sockfd,(struct sockaddr*)&addr, sizeof(addr));
    if(ret<0)
    {
        std::cout<<"Socket:bind error"<<strerror(errno)<<std::endl;
        exit(1);
    }
}

void Socket::Close(const int sockfd)
{
    if(close(sockfd)<0)
    {
        std::cout<<"Socket:close error"<<strerror(errno)<<std::endl;
        exit(1);
    }
}


int Socket::createSocket()
{
    int sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock<0)
    {
        std::cout<<"Socket:create error"<<strerror(errno)<<std::endl;
        exit(1);
    }
    return sock;
}

void Socket::Listen(const int sockfd)
{
    if(listen(sockfd,5)<0)
    {
        std::cout<<"Socket:listen error"<<strerror(errno)<<std::endl;
        exit(1);
    }
}

void Socket::setReuseAddr(const int fd, bool on)
{
    int opt;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt, sizeof(opt));
}


//将套接字设置为非阻塞的模式
//成功返回sock
//失败返回-1
int Socket::setNonblock(int sock)
{
    int flags = -1;
    int err = -1;
    flags = fcntl(sock,F_GETFL,0);
    if (flags < 0)
    {
        std::cout<<"setNonblock:fcntl F_GETFL failed"<<std::endl;
        return -1;
    }
    //不直接设置O_NONBLOCK，防止将套接字之前的属性覆盖，所以有一个判断
    if (!(flags&O_NONBLOCK))
    {
        flags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, flags) < 0)
        {
            std::cout<< "setNonblock:fcntl F_SETFL failed" <<std:: endl;
            return -1;
        }
    }
    return sock;

}
