
#ifndef WEBSRV_EVENTLOOP_H
#define WEBSRV_EVENTLOOP_H
#include <sys/syscall.h>
#include <sys/types.h>
#include <assert.h>
#include <vector>
#include <poll.h>
#include <signal.h>
#include "Epoll.h"
#include "CurrentThread.h"

class Handler;


//事件的循环类，包括将fd添加到vector中，然后再添加到epoll中
class EventLoop
{
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void addToLoop(const int fd);

private:
    void addToLoop();
    bool isLooping;
    const pid_t threadId;
    std::vector<int> fds;
    bool isQuit;
    Epoll *e;
};
#endif //WEBSRV_EVENTLOOP_H
