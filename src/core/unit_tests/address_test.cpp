//address_test.cpp

#define BOOST_TEST_MODULE core
#include <boost/test/unit_test.hpp>

#include "core/sn_address.h"
#include <iostream>

using namespace snode;
using namespace std;

BOOST_AUTO_TEST_SUITE(Address_test)
BOOST_AUTO_TEST_CASE( test_1)
{
	BOOST_TEST_INFO("Constructor and size() sn() en()");
	Address addr(0x1000u, 0x12345u);
	BOOST_TEST(addr.size()==64/8);
	BOOST_TEST(addr.sn()==0x1000);
	BOOST_TEST(addr.en()==0x12345);
	BOOST_TEST_MESSAGE("addr is "<<addr);

	BOOST_TEST_INFO("operator != < and ==");
	Address addr2(0x1001u, 0x12345u);
	BOOST_TEST((addr != addr2));
	Address addr3(0x1000u, 0x12346u);
	BOOST_TEST((addr != addr3));
	BOOST_TEST((addr < addr3));
	Address addr4(0x1000u, 0x12345u);
	BOOST_TEST((addr == addr4));

	BOOST_TEST_INFO("raw() and rowton()");
	Address addr5(addr4.raw());
	BOOST_TEST((addr5 == addr4));
	Address addr6 = addr4.raw();
	BOOST_TEST((addr6 == addr4));	
	uint64_t rawn=addr4.rawton();
	Address addr7(rawn, true);
	BOOST_TEST((addr7 == addr4));
	Address addr8(0x02u, 0x01u);
	uint64_t raw = 0x02ul<<32| 0x01;
	BOOST_TEST(addr8.raw() == raw);
    BOOST_TEST_MESSAGE("addr8:"<<hex<<raw);

}
BOOST_AUTO_TEST_SUITE_END()