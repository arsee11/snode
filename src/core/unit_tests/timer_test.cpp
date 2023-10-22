//timer_test.cpp 

#include <boost/test/unit_test.hpp>

#include "core/sn_timer.h"
#include <string.h>

using namespace snode;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(timer_test)
BOOST_AUTO_TEST_CASE( test_1)
{
    Timer timer;
    timer.start(3000, []{
        printf("time out\n");
    });
	BOOST_TEST(true);	
}

BOOST_AUTO_TEST_SUITE_END()