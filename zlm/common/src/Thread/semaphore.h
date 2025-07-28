#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <mutex>
#include <condition_variable>

namespace ysh_toolkit
{
class Semaphore {
public:
    explicit semaphore(size_t initial = 0){
        sem_init(&_sem, 0, initial);
    }

    ~semaphore(){
        sem_destroy(&_sem);
    }

    void post(size_t n = 1){
        while(n -- ){
            sem_post(&_sem);
        }
    }

    void wait(){
        sem_wait(&_sem);
    }

private:
    sem_t _sem;

};

#endif