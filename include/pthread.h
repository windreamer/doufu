#ifndef __PTHREAD_H__
#define __PTHREAD_H__
#include "noncopyable.h"
#include <pthread>
#include <stdexcept>

namespace doufu
{

    class mutex : private noncopyable
    {
    public:
        mutex(int mutex_type = PTHREAD_MUTEX_RECURSIVE)
        {
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, mutex_type); 
            if(0 != pthread_mutex_init(&m, &attr))
            {
                throw std::runtime_error("fail to init mutex");
            }
        }

        ~mutex()
        {
            pthread_mutex_destroy(&m);
            pthread_mutexattr_destroy(&attr);
        }

        void lock()
        {
            pthread_mutex_lock(&m);
        }

        void unlock()
        {
            pthread_mutex_unlock(&m);
        }

        
        pthread_t* get()
        {
            return &m;
        }
    private:
        pthread_mutexattr_t attr;
        pthread_mutex_t m

    };

    class scoped_lock : private noncopyable
    {
    public:
        scoped_lock(mutex& m_): mutex(m_)
        {
            m.lock()
        }

        ~scoped_lock()
        {
            m.unlock();
        }
    private:
        mutex& m;
    }

    class condition : private noncopyable
    {
    public:
        condition(): cancel(false)
        {
            if(0 != pthread_cond_init(&cond, NULL))
            {
                throw std::runtime_error("fail to init mutex");
            }
        }

        ~condition()
        {
            pthread_cond_destroy(&cond);
        }

        void wait (mutex& m)
        {
            pthread_cond_wait(&cond, m.get());
        }

        void signal(bool broadcast = false)
        {
            if(broadcast)
            {
                pthread_cond_broadcast(&cond);
            }
            else
            {
                pthread_cond_signal(&cond);
            }
        }
    private:
        pthread_cond_t cond;
    }

}

#endif

