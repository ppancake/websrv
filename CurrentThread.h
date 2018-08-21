
#ifndef WEBSRV_CURRENTTHREAD_H
#define WEBSRV_CURRENTTHREAD_H

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>

class CurrentThread
{
public:
    //获取当前线程id
    static pid_t gettid()
    {
        return static_cast<pid_t>(syscall(SYS_gettid));
    }
};


#endif //WEBSRV_CURRENTTHREAD_H
