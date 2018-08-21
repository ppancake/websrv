

#ifndef WEBSRV_EPOLL_H
#define WEBSRV_EPOLL_H
#include <sys/epoll.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "Handler.h"

using namespace std;
class EventLoop;

class Epoll
{
public:
    explicit Epoll(EventLoop *loop) :ownerLoop(loop), epollfd(epoll_create1(EPOLL_CLOEXEC)), retEvents(16)
    {

        if(epollfd < 0)
            std::cout << "Epoll::epoll_create1() error: " << ::strerror(errno) << std::endl;
        assert(epollfd > 0);
    }

    ~Epoll()
    { close(epollfd); }

    // 调用epoll_wait，并将其交给Event类的handleEvent函数处理
    void epoll(std::vector<Handler*> &acevents);
    void removeFd(const int fd);
    void addToEpoll(const int fd);

private:
    EventLoop *ownerLoop;
    int epollfd;
    std::vector<struct epoll_event> retEvents;
};


#endif //WEBSRV_EPOLL_H
