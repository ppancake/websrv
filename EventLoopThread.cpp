#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() : m_loop(new EventLoop()), m_thread(threadFunc, m_loop)
{
    m_thread.start();
}

EventLoopThread::~EventLoopThread()
{
    m_loop->quit();
    m_thread.join();
}

//loop(): 调用epoll将活动的套接字描述符对应的Handler取出并处理
void* EventLoopThread::threadFunc(void *arg)
{
    EventLoop *loop = (EventLoop*)arg;
    loop->loop();
}
