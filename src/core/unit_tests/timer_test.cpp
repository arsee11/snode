//timer_test.cpp 

#include <boost/test/unit_test.hpp>

#include "core/sn_timer.h"
#include <string.h>

using namespace snode;
namespace utf = boost::unit_test;

using timer_ptr = std::unique_ptr<Timer>;


static EventQueueEpoll eq;
static ThreadScopePolling t(&eq);


BOOST_AUTO_TEST_SUITE(timer_test)
BOOST_AUTO_TEST_CASE( test_1)
{
    std::promise<int> p;
    auto f = p.get_future();
    timer_ptr timer(
        Timer::start(1000, [&p](int state){
            BOOST_TEST(state == Timer::Timeout);
            static int count=0;
            count++;
            if(count > 2){
                p.set_value(count);                
            }
            printf("test_1 time out\n");            
        }, &eq)
    );
    int c = f.get();
	BOOST_TEST(c < 4);	
}

BOOST_AUTO_TEST_CASE( test_2)
{
    timer_ptr timer(Timer::start(1000, &eq));
    int state = timer->wait();
    BOOST_TEST((state == Timer::Timeout));
    printf("test_2 time out\n");
    timer->stop();
    printf("test_2 timer stop\n");
    
    state = timer->wait();
    BOOST_TEST(state == Timer::Invalid);

	BOOST_TEST(true);	
}

AwaitTimer::co_task do_timer(AwaitTimer& timer){
    for(int i=0; i<5; i++){
        int e = co_await timer.await(1000);
        printf("test_3 %s %d\n", e==Timer::Timeout?"time out":"cancel", i);
    }
}

BOOST_AUTO_TEST_CASE( test_3)
{
    AwaitTimer timer(&eq);
        
    do_timer(timer);
    printf("test_3 go on...\n");
    getchar();
    timer.reset();
    getchar();
    getchar();
    timer.stop();
	BOOST_TEST(true);	
}

BOOST_AUTO_TEST_CASE( test_4)
{
    timer_ptr timer(Timer::start(3000, &eq));

    std::thread t([&]{
        std::this_thread::sleep_for(1s);
        timer->stop();
        printf("test_4 stop timer\n");
    });
    int state = timer->wait();
    BOOST_TEST((state == Timer::Invalid));
    printf("test_4 time out\n");
    t.join();
    
}

BOOST_AUTO_TEST_SUITE_END()