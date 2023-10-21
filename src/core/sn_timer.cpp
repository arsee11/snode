///sn_timer.cpp

#include "sn_timer.h"
#include <sys/timerfd.h>


void snode::Timer::start(int interval_ms, timeout_cb)
{
    if(fd > 0){
        stop();
    }

    fd = timerfd_create(CLOCK_REALTIME, 0);

    itimerspec new_value;
    new_value.it_value.tv_sec = interval_ms/1000;
    new_value.it_value.tv_nsec = (interval_ms%1000)*1000000;
    new_value.it_interval.tv_sec = interval_ms/1000;
    new_value.it_interval.tv_nsec = (interval_ms%1000)*1000000;
    if(timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, nullptr) == -1){
        std::__throw_runtime_error("timer start failed.");
    }


     epoll.addInput(fd, &timer_event);

    while(true){
        auto evts = epoll.select();
        if(evts.empty()){
            continue;
        }
        for(auto evt : evts){
            uint64_t exp=0;
            int n = read(evt->fd, &exp, sizeof(exp));
            if( n < sizeof(exp)){
                perror("read");
                return;
            }

            
            for(int i=0; i<exp; i++){
                if(timeout_cb != nullptr){
                    timeout_cb();
                }
            }
        }
    }

}

void snode::Timer::stop()
{
    epoll.detach(fd);
    close(fd);
    fd=-1;
}
