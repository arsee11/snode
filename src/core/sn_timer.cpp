///sn_timer.cpp

#include "sn_timer.h"
#include <sys/timerfd.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

snode::Timer::Timer()
{
    _efd = epoll_create(1024);
    if( _efd == -1 )
    {
        perror("epoll_create");
        throw std::exception();
    }
}

void snode::Timer::start(int interval_ms, timeout_cb cb)
{
    if(fd > 0){
        stop();
    }

    fd = timerfd_create(CLOCK_REALTIME, 0);

    epoll_event eh;
	eh.events = EPOLLET|EPOLLIN;
    eh.data.fd = fd;
	if( epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &eh) == -1){
		//error handle
		perror("epoll_ctl");
		return;
	}

    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    itimerspec new_value;
    new_value.it_value.tv_sec = now.tv_sec + interval_ms/1000;
    new_value.it_value.tv_nsec = now.tv_nsec + (interval_ms%1000)*1000000;
    new_value.it_interval.tv_sec = interval_ms/1000;
    new_value.it_interval.tv_nsec = (interval_ms%1000)*1000000;
    if(timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, nullptr) == -1){
        std::__throw_runtime_error("timer start failed.");
    }



    while(true){
        epoll_event ehs[1024];
        int nfds = epoll_wait(_efd, ehs, 1024, 500);
        std::cout<<"selected nfds:"<<nfds<<"\n";
        if(nfds == -1){
            perror("epoll_wait");
            continue; 
        }

        for(int i=0; i<nfds; ++i){			
            uint64_t exp=0;
            int n = read(ehs[i].data.fd, &exp, sizeof(exp));
            std::cout<<"read "<<n<<" bytes, value:"<<exp<<"\n";
            if( n < sizeof(exp)){
                perror("read");
                return;
            }

            
            for(int i=0; i<exp; i++){
                if(cb != nullptr){
                    cb();
                }
            }
        }
    }

}

void snode::Timer::stop()
{
    epoll_ctl(_efd, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
    fd=-1;
}
