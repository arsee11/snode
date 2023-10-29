///sn_timer.cpp

#include "sn_timer.h"
#include <sys/timerfd.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

namespace snode
{

class CallBackTimer : public Timer
{
public:
    CallBackTimer(timeout_cb cb, EventQueueEpoll* eq)
        :Timer(eq)
        ,_timeout_cb(cb)
    {}

private:
    void onTimeout()override{
         if(_timeout_cb != nullptr){
                    _timeout_cb();
        }
    }
    timeout_cb _timeout_cb;
};

class PromiseTimer : public Timer
{
public:
    PromiseTimer( EventQueueEpoll* eq)
        :Timer(eq)
    {
        _future = _promise.get_future();
    }

private:
    void onTimeout()override{
        _promise.set_value();
    }
    int wait()override{
        if(_is_running){
            _future.wait();
            return Timeout;
        }
        else{
            return Invalid;
        }
    }

    std::promise<void> _promise;
    std::future<void> _future;
};


Timer::Timer(EventQueueEpoll* eq)
    :_exec_queue(eq)
{    

}

Timer::~Timer()
{
    if(_is_running){
        stop();
    }
}

Timer* Timer::start(int interval_ms, timeout_cb cb, EventQueueEpoll* eq)
{
    Timer* timer = new CallBackTimer(cb, eq);
    if(timer->start_i(interval_ms)){
        return timer;
    }

    delete timer;
    return nullptr;
}

Timer *Timer::start(int interval, EventQueueEpoll* eq)
{
    Timer* timer = new PromiseTimer(eq);
    if(timer->start_i(interval)){
        return timer;
    }

    delete timer;
    return nullptr;
}

bool Timer::start_i(int interval_ms)
{
    _fd = timerfd_create(CLOCK_REALTIME, 0);

    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    itimerspec new_value;
    new_value.it_value.tv_sec = now.tv_sec + interval_ms/1000;
    new_value.it_value.tv_nsec = now.tv_nsec + (interval_ms%1000)*1000000;
    new_value.it_interval.tv_sec = interval_ms/1000;
    new_value.it_interval.tv_nsec = (interval_ms%1000)*1000000;
    if(timerfd_settime(_fd, TFD_TIMER_ABSTIME, &new_value, nullptr) == -1){
        std::__throw_runtime_error("timer start failed.");
    }

    _exec_queue->bindInput(_fd, std::bind(&Timer::loop, this));
    _is_running = true;

    return true;
}

void Timer::loop()
{		
    uint64_t exp=0;
    int n = read(_fd, &exp, sizeof(exp));
    //std::cout<<"read "<<n<<" bytes, value:"<<exp<<"\n";
    if( n < sizeof(exp)){
        perror("read");
        return;
    }

    for(int i=0; i<exp; i++){
        onTimeout();               
    }
}

void Timer::stop()
{
    _exec_queue->unbind(_fd);
    close(_fd);
    _fd=-1;   
    _is_running = false; 
}

}//snode