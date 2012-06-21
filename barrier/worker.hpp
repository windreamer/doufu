#ifndef __WORKER_HPP__
#define __WORKER_HPP__
#include "block_queue.hpp"
#include "msgpack.hpp"
namespace doufu
{
    class worker 
    {
    public:
        typedef block_queue<std::pair<uint32_t, msgpack::object> > bqueue;
        worker(bqueue&, bqueue&);
        void* run();
    private:
        bqueue& task_queue;
        bqueue& resp_queue;
    };
}
#endif

