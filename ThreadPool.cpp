#include "ThreadPool.h"

ThreadPool::ThreadPool(const int threadNum)
        : m_threadNum(threadNum),
          m_nextID(0)
{
    for(int i = 0; i < threadNum; ++i)
    {
        EventLoopThread *thread = new EventLoopThread();
        m_threads.push_back(thread);
    }
}

ThreadPool::~ThreadPool()
{
    for(int i = 0; i < m_threads.size(); ++i)
        delete m_threads[i];
}

// 每次以轮转的方式取一个工作线程
EventLoopThread* ThreadPool::getNextThread()
{
    if(m_nextID >= m_threadNum)
        m_nextID = 0;
    return m_threads[m_nextID++];
}