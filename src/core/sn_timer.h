///sn_timer.h

#ifndef SN_TIMER_H
#define SN_TIMER_H

#include <functional>
#include <errno.h>
#include <unistd.h>
#include <future>
#include "sn_threading.h"
#include <coroutine>

namespace snode
{

/// @brief 
class Timer
{
protected:
    using timeout_cb = std::function<void()>;

public:
    virtual ~Timer();

    static Timer* start(int interval, timeout_cb, EventQueueEpoll* eq);
    static Timer* start(int interval, EventQueueEpoll* eq);
    void stop();

    enum{
        Timeout,
        Invalid
    };
    virtual int wait(){ throw std::runtime_error("can't wait.");}

protected:
    Timer(EventQueueEpoll* eq);
    void loop();
    virtual void onTimeout()=0;
    bool start_i(int interval_ms);

    int _fd=-1;
    EventQueueEpoll* _exec_queue =nullptr;
    std::atomic_bool _is_running;
};

class AwaitTimer : public Timer
{

    struct CoTask{
        struct promise_type {
            CoTask get_return_object() { return {};}
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() {}

        };
    };

    friend struct Awaiter;
    struct Awaiter{
            bool await_ready(){
                return false;
            }

            void await_suspend(std::coroutine_handle<> awaiting_coro){
                _timer->_coro_h = awaiting_coro;
            }

            void await_resume(){
                _timer->stop();
            }  

            AwaitTimer* _timer=nullptr;    
    };

public:
    AwaitTimer( EventQueueEpoll* eq)
        :Timer(eq)
    {
    }

    using co_task = CoTask;
    Awaiter await(int interval_ms){
        start_i(interval_ms);
        return Awaiter(this);
    }

private:
    void onTimeout()override{
        _coro_h.resume();
    }

    std::coroutine_handle<> _coro_h;
};

}

#endif /*SN_TIMER_H*/
