#include <iostream>
#include "cstdlib"
#include "Socket.h"
#include "Handler.h"
#include "ThreadPool.h"
#include "EventLoop.h"

using namespace std;
int main(int argc,char **argv)
{

    int port=7070;

    int listenFd = Socket::createSocket();
    Socket::setReuseAddr(listenFd, true);
    struct sockaddr_in srvaddr;
    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvaddr.sin_port = htons(port);
    Socket::Bind(listenFd, srvaddr);
    Socket::Listen(listenFd);
    ThreadPool*threadPool=new ThreadPool(10);

    while(true)
    {
        struct sockaddr_in cliaddr;
        socklen_t cliaddrLen = sizeof(cliaddr);
        memset(&cliaddr, 0, sizeof(cliaddr));
        int connFd = Socket::Accept(listenFd, cliaddr);

        // 挑选一个线程，将已连接套接字注册到此线程的EventLoop中
        EventLoopThread *thread = threadPool->getNextThread();
        EventLoop *loop = thread->getLoop();
        loop->addToLoop(connFd);
    }

    Socket::Close(listenFd);
    delete threadPool;

    return 0;

}

