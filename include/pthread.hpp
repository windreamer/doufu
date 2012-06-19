#ifndef __PTHREAD_HPP__
#define __PTHREAD_HPP__
#include "noncopyable.hpp"
#include <memory>
#include <pthread.h>
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

        virtual ~mutex()
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


        pthread_mutex_t* get()
        {
            return &m;
        }
    private:
        pthread_mutexattr_t attr;
        pthread_mutex_t m;

    };

    class scoped_lock : private noncopyable
    {
    public:
        scoped_lock(mutex& m_): m(m_)
        {
            m.lock();
        }

        virtual ~scoped_lock()
        {
            m.unlock();
        }
    private:
        mutex& m;
    };

    class condition : private noncopyable
    {
    public:
        condition()
        {
            if(0 != pthread_cond_init(&cond, NULL))
            {
                throw std::runtime_error("fail to init mutex");
            }
        }

        virtual ~condition()
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
    };

    class thread
    {
    public:
        template<typename F>
        thread(const F& f, typename F::argument_type a)
        {
            typedef typename F::argument_type A;
            typedef context<F, A> context_type;

            std::auto_ptr<context_type> thread_context (
                    new context_type(f, a));
            int ret = pthread_create (
                    &handle, NULL, context_type::start,
                    thread_context.get());
            if(ret)
            {
                throw std::runtime_error("failed to create thread");
            }
            thread_context.release();
        }

        thread (const thread& rhs): handle(rhs.handle)
        {
        }

        virtual ~thread()
        {
        }

        static thread current()
        {
            return thread(pthread_self());
        }

        thread& operator = (const thread& rhs)
        {
            if (&rhs == this)
            {
                return *this;
            }
            handle = rhs.handle;
            return *this;
        }

        bool operator == (const thread& rhs) const
        {
            return pthread_equal(handle, rhs.handle) != 0;
        }

        bool operator != (const thread& rhs) const
        {
            return !(operator==(rhs));
        }

        void* join() const
        {
            void* result = NULL;
            int ret = pthread_join(handle, &result);
            if(ret)
            {
                throw std::runtime_error("pthread_join failed");
            }

            return result;
        }

        void detach() const
        {
            int ret = pthread_detach(handle);
            if(ret)
            {
                throw std::runtime_error("pthread_detach failed");
            }
        }

    private:
        template<typename F, typename A>
        class context: private noncopyable
        { 
        public:
            context(const F& f, A a): functor(f), arg(a)
            {
            }

            static void* start(void* self_)
            {
                std::auto_ptr<context> self(static_cast<context*>(self_));
                return reinterpret_cast<void*>(self->functor(self->arg));
            }
        private:
            F functor;
            A arg;
        };

        explicit thread (const pthread_t handle_): handle(handle_)
        {
        }

        pthread_t handle;
    };

}

#endif

