

#ifndef WEBSRV_CONDITION_H
#define WEBSRV_CONDITION_H

#include <pthread.h>


//简单的封装。
class Condition
{
public:
    Condition()
    {
        pthread_mutex_init(&mutex,NULL);
        pthread_cond_init(&cond,NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&cond);
    }

    void wait()
    {
        pthread_cond_wait(&cond, &mutex);
    }

    void notify()
    {
        pthread_cond_signal(&cond);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&cond);
    }

private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};



#endif //WEBSRV_CONDITION_H
