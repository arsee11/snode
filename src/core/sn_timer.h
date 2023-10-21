///sn_timer.h

#ifndef SN_TIMER_H
#define SN_TIMER_H

#include <functional>
#include "arseeulib/threading/epoll.h"

namespace snode
{

/// @brief 
class Timer
{
    using timeout_cb = std::function<void()>;
    struct TimerEvent
    {
        int fd;
    };

public:
    Timer() =default;
    void start(int interval, timeout_cb);
    void stop();

private:
    int fd=-1;
    TimerEvent timer_event;
    arsee::net::Epoll<TimerEvent> epoll;
};


}

#endif /*SN_TIMER_H*/
