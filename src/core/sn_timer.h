///sn_timer.h

#ifndef SN_TIMER_H
#define SN_TIMER_H

#include <functional>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace snode
{

/// @brief 
class Timer
{
    using timeout_cb = std::function<void()>;

public:
    Timer();
    void start(int interval, timeout_cb);
    void stop();

private:
    int fd=-1;
    int _efd;
};


}

#endif /*SN_TIMER_H*/
