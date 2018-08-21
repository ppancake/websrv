#include "Epoll.h"

void Epoll::epoll(std::vector<Handler *> &acevents)
{
    int numEvents=epoll_wait(epollfd,retEvents.data(), static_cast<int>(retEvents.size()),100);
    if(numEvents<0)
    {
        if(errno!=EINTR)
        {
            cout<<"Epoll wait error"<<endl;
            exit(1);

        }
    }
    else if(numEvents==0)
    {

    }
    else
    {
        for(int i=0;i<numEvents;i++)
        {
            Handler *h=new Handler(retEvents[i].data.fd);
            acevents.push_back(h);
        }
    }


}

void Epoll::addToEpoll(const int fd)
{
    epoll_event event;
    event.data.fd=fd;
    event.events = EPOLLIN | EPOLLPRI | EPOLLRDHUP;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) < 0)
        std::cout << "Epoll::addToEpoll error: " << strerror(errno) << std::endl;
}

void Epoll::removeFd(const int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}