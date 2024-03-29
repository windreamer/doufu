#ifndef __BLOCK_QUEUE_HPP__
#define __BLOCK_QUEUE_HPP__
#include "noncopyable.hpp"
#include "pthread.hpp"
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
            clear();
        }

        void clear()
        {
            scoped_lock lock(queue_mutex);
            while(!queue.empty())
            {
                queue.pop();
            }
            stop = true;
            cond.signal(true);
        }

        bool empty()
        {
            scoped_lock lock(queue_mutex);
            return queue.empty();
        }

        size_t size()
        {
            scoped_lock lock(queue_mutex);
            return queue.size();
        }

        bool pop(value_type& value)
        {
            scoped_lock lock(queue_mutex);
            while(queue.empty() && !stop)
            {
                cond.wait(queue_mutex);
            }

            if(stop)
            {
                return false;
            }

            value = queue.front();
            queue.pop();
            return true;
        }

        value_type& push(const value_type& t)
        {
            scoped_lock lock(queue_mutex);
            if(stop)
            {
                return;
            }

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
        condition cond;
    };
}
#endif

