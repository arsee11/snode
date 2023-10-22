
#include <boost/test/included/unit_test.hpp>
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(rip_routing_test)

BOOST_AUTO_TEST_CASE(test1)
{
  BOOST_TEST(false);
  BOOST_CHECK(1==2);
  BOOST_CHECK(2==2);
  BOOST_REQUIRE(2==2);
}

BOOST_AUTO_TEST_SUITE_END()