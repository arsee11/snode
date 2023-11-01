
#include <boost/test/unit_test.hpp>

#include "core/sn_route_table.h"
#include <string.h>

using namespace std;
using namespace snode;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(route_table)
BOOST_AUTO_TEST_CASE( test_1, * utf::depends_on("Address_test/test_1"))
{

	RouteTable rt;
	BOOST_TEST_INFO("empty table");
	BOOST_TEST((rt.routing(Address(1u, 0u), 16) == nullptr));
	BOOST_TEST((rt.find_best_by_dst(Address(1u, 0u)) == nullptr));
	BOOST_TEST((rt.getAllItems().size() == 0));

	port_ptr port1(new Port);
	port_ptr port2(new Port);
	port_ptr port3(new Port);
	port_ptr port4(new Port);
	rt.add(Address(1u, 0u), 1, Address(1u, 1u), port1);
	rt.add(Address(2u, 0u), 2, Address(2u, 1u), port2);
	rt.add(Address(3u, 0u), 3, Address(1u, 1u), port3);
	rt.add(Address(4u, 0u), 4, Address(2u, 1u), port4);

	BOOST_TEST_INFO("not empty table");
	BOOST_TEST((rt.routing(Address(1u, 0u), 16) == port1));
	BOOST_TEST((rt.routing(Address(2u, 0u), 16) == port2));
	BOOST_TEST((rt.routing(Address(3u, 0u), 16) == port3));
	BOOST_TEST((rt.routing(Address(4u, 0u), 16) == port4));
	BOOST_TEST((rt.routing(Address(4u, 0u), 1) == nullptr));
	
	auto item = rt.find_best_by_dst(Address(1u, 0u));
	BOOST_TEST( (item->next_hop == Address(1u, 1u)) );
	BOOST_TEST( (item->metric == 1) );
	item = rt.find_best_by_dst(Address(4u, 0u));
	BOOST_TEST( (item->next_hop == Address(2u, 1u)) );
	BOOST_TEST( (item->metric == 4) );

	BOOST_TEST((rt.getAllItems().size() == 4));

	
}
BOOST_AUTO_TEST_SUITE_END()