
#ifndef WEBSRV_EVENTLOOPTHREAD_H
#define WEBSRV_EVENTLOOPTHREAD_H

#include "EventLoop.h"
#include "Thread.h"


//将线程和事件循环结合起来。循环线程
class EventLoopThread
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* getLoop()
    {
        return m_loop;
    }

private:
    static void* threadFunc(void *arg);
    EventLoop *m_loop;
    Thread m_thread;
};
#endif //WEBSRV_EVENTLOOPTHREAD_H
