#include "EventLoop.h"


void IgnoreSigPipe()
{
    signal(SIGPIPE, SIG_IGN);
}



EventLoop::EventLoop()
        :isLooping(false),
         threadId(CurrentThread::gettid()),
         isQuit(false),
         e(new Epoll(this))
{ }

EventLoop::~EventLoop()
{
    assert(!isLooping);
}

void EventLoop::loop()
{
    assert(!isLooping);
    isLooping = true;
    isQuit = false;

    while(!isQuit)
    {
        addToLoop();
        std::vector<Handler*> activeEvents;
        activeEvents.clear();

        // 调用epoll将活动的套接字描述符对应的Handler取出并处理
        e->epoll(activeEvents);
        for(std::vector<Handler*>::iterator iter = activeEvents.begin();
            iter != activeEvents.end(); ++iter)
        {
            // 处理客户端请求的入口
            (*iter)->handle();
            e->removeFd((*iter)->getfd());
            delete *iter;
        }
    }
    isLooping = false;
}

void EventLoop::quit()
{
    isQuit = true;
}

// 每次将一个套接字描述符添加到描述符数组中
void EventLoop::addToLoop(const int fd)
{

    fds.push_back(fd);
}

// 将描述符数组中的所有套接字全部添加到epoll中
void EventLoop::addToLoop()
{
    if(fds.empty())
    {
        return;
    }
    for(int i = 0; i < fds.size(); ++i)
        e->addToEpoll(fds[i]);
    fds.clear();
}

