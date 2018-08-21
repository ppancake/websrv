#include "Thread.h"
#include <assert.h>
void Thread::start()
{
    assert(!started);
    started = true;
    if(pthread_create(&pthreadId, NULL, threadFunc, _arg))
        started = false;
}

int Thread::join()
{
    assert(started && !joined);
    joined = true;
    return pthread_join(pthreadId, NULL);
}