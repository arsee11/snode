
#include <boost/test/unit_test.hpp>

#include "core/sn_address_manager.h"
#include <string.h>

using namespace std;
using namespace snode;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(address_manager)
BOOST_AUTO_TEST_CASE( test_1, * utf::depends_on("Address_test/test_1"))
{

	AddressManager am(1000);
	BOOST_TEST_INFO(" alloc address");
	auto addr1 = am.allocAddress();
	BOOST_TEST(addr1.sn() == 1000);
	BOOST_TEST((addr1.en() > 1 && addr1.en()<AddressManager::MAX ));
	BOOST_TEST_MESSAGE("addr1 is "<<addr1);

	auto addr2 = am.allocAddress();
	BOOST_TEST(addr2.sn() == 1000);
	BOOST_TEST((addr1 != addr2)) ;
	BOOST_TEST_MESSAGE("addr2 is "<<addr2);

	BOOST_TEST_INFO(" alloc address till full");
	try{
		for(int i=0; i< AddressManager::MAX+1; i++){
			am.allocAddress();
		}
		BOOST_TEST(false);
	}
	catch(const AddressFullException& e){
		BOOST_TEST(true);
	}

	BOOST_TEST_INFO(" release invalid address");
	try{
		am.releaseAddress(Address(1000u, 1025));
		BOOST_TEST(false);
	}
	catch(const InvalidAddressException& e){
		BOOST_TEST(true);
	}

	BOOST_TEST_INFO(" release address and realloc one");
	am.releaseAddress(addr1);
	auto addr3 = am.allocAddress();
	BOOST_TEST(addr3.sn() == 1000);
	BOOST_TEST((addr3 == addr1));
	BOOST_TEST((addr3 != addr2));
	BOOST_TEST_MESSAGE("addr3 is "<<addr3);


}
BOOST_AUTO_TEST_SUITE_END()