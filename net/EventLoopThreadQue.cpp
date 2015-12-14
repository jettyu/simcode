#include <simcode/net/EventLoopThreadQue.h>

using namespace simcode;
using namespace net;

int EventLoopThreadQue::addTask(const TaskCallback& cb)
{
    if (thread_que_->push_back(cb) != 0)
    {
        EventLoopThreadPtr tp = task_que_.Get();
        if (tp)
        {
            tp->loop()->addTask(simex::bind(&EventLoopThreadQue::callback, this, tp, cb));
        }
        else
        {
            return default_que_->push_back(cb);
        }
    }
    return 0;
}
