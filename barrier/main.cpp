#include "block_queue.hpp"
#include "server.hpp"
#include "pthread.hpp"
#include "worker.hpp"
#include <ev++.h>
#include <iostream>
#include <msgpack.hpp>
#include <vector>
using namespace doufu;
using namespace std;

struct sig_handler
{
    void handle(ev::sig &signal, int revents)
    {
        signal.loop.break_loop();
    }
};

int main ()
{
    sig_handler sig;
    ev::sig sio;
    sio.set<sig_handler, &sig_handler::handle>(&sig);
    sio.start(SIGINT);

    block_queue<pair<uint32_t, msgpack::object> > task_queue;
    block_queue<pair<uint32_t, msgpack::object> > resp_queue;

    vector<thread> workers;
    for(int i=0; i < 10; ++i)
    {
        worker w(task_queue, resp_queue);
        workers.push_back(thread(mem_fun_ref(&worker::run), w));
    }


    ev::default_loop loop;
    loop.run();
    task_queue.clear();
    resp_queue.clear();

    for(int i=0; i < 10; ++i)
    {
        workers[i].join();
    }

    cout<<"barrier exited..."<<endl;
    return 0;
}
