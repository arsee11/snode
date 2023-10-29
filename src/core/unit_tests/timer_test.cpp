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
        Timer::start(1000, [&p]{
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
        
    state = timer->wait();
    BOOST_TEST(state == Timer::Invalid);

	BOOST_TEST(true);	
}

AwaitTimer::co_task do_after(int ms, AwaitTimer& timer){
    co_await timer.await(ms);
    printf("test_3 time out\n");

}

BOOST_AUTO_TEST_CASE( test_3)
{
    AwaitTimer timer(&eq);
        
    do_after(1000, timer);
    printf("test_3 go on...\n");
    getchar();
	BOOST_TEST(true);	
}

BOOST_AUTO_TEST_SUITE_END()