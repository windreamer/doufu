#include "worker.hpp"
#include <iostream>
using namespace doufu;
using namespace std;

worker::worker(bqueue& task_, bqueue& resp_)
    : task_queue(task_), resp_queue(resp_)
{
}

void* worker::run()
{
    cout<<"worker started"<<endl;
    bqueue::value_type task;
    while(task_queue.pop(task))
    {

    }
    cout<<"worker stopped"<<endl;
    return 0;
}
