
#ifndef WEBSRV_THREADPOOL_H
#define WEBSRV_THREADPOOL_H

#include "EventLoop.h"
#include <vector>
#include "EventLoopThread.h"

class EventLoop;
class EventLoopThread;
class ThreadPool
{
public:
    ThreadPool(int threadNum);
    ~ThreadPool();
    EventLoopThread* getNextThread();
private:
    int m_threadNum;
    int m_nextID;
    EventLoop *m_loop;
    std::vector<EventLoopThread*> m_threads;

};


#endif //WEBSRV_THREADPOOL_H
