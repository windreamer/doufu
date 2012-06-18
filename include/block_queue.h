#ifndef __BLOCK_QUEUE_H__
#define __BLOCK_QUEUE_H__
#include "noncopyable.h"
#include "pthread.h"
#include <queue>

namespace doufu
{
    template <typename T>
        class block_queue : private noncopyable
    {
    public:
        typedef T value_type;

        block_queue(): stop(false) {}
        ~block_queue()
        {
            stop = true;
            cond.signal(true);
        }

        bool empty()
        {
            scoped_lock(queue_mutex);
            return queue.empty();
        }

        size_t size()
        {
            scoped_lock(queue_mutex);
            return queue.size();
        }

        value_type& front()
        {
            scoped_lock(queue_mutex);
            return queue.front();
        }

        value_type& back()
        {
            scoped_lock(queue_mutex);
            return queue.back();
        }

        void pop()
        {
            scoped_lock(queue_mutex);
            if(queue.empty() && !stop)
            {
                cond.wait(queue_mutex);
            }
            if(!queue.empty())
            {
                queue.pop();
            }
        }

        void push(const value_type& t)
        {
            scoped_lock(queue_mutex);
            queue.push(t);
            if(1 == queue.size())
            {
                cond.signal();
            }
        }

    private:
        bool stop;
        std::queue<value_type> queue;
        mutex queue_mutex;
        condtion cond;
    };
}
#endif

