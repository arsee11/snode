//message_test.cpp 

#include <boost/test/unit_test.hpp>

#include "core/sn_message.h"
#include <string.h>

using namespace std;
using namespace snode;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(Message_test)
BOOST_AUTO_TEST_CASE( test_1, * utf::depends_on("Address_test/test_1"))
{
	Address src(0x0001ul, 0x1234ul);
	Address dst(0x0001ul, 0x2345ul);
	const char* m="hello,world!";
	Message msg(src, dst, m, strlen(m));
	BOOST_TEST(msg.payload_type()==Message::NormalMsg);

	uint8_t* stream;
	size_t size;
	std::tie(stream, size) = msg.encode();

	BOOST_TEST(msg.header_size() == 25);
	BOOST_TEST(strlen(m)+msg.header_size() == msg.size());
	BOOST_TEST(size == msg.size());
	BOOST_TEST(strlen(m) == msg.payload_size());
	BOOST_TEST( strcmp((char*)msg.payload(), m)==0 );

	Message msg2;
	msg2.decode(stream, size);

	BOOST_TEST( msg.payload_type()==Message::NormalMsg);
	BOOST_TEST((msg.dst() == msg2.dst()));
	BOOST_TEST((msg.src() == msg2.src()));
	BOOST_TEST(msg.payload_size() == msg2.payload_size());
	BOOST_TEST(strcmp((char*)msg.payload(), (char*)msg2.payload())==0 );
	
}

BOOST_AUTO_TEST_CASE( test_2, * utf::depends_on("Address_test/test_1"))
{
	Address src(0x0001ul, 0x1234ul);
	Address dst(0x0001ul, 0x2345ul);
	const char* m="hello,world!";
	Message msg(src, dst, Message::LinkUpdateMsg, m, strlen(m));

	BOOST_TEST( msg.payload_type()==Message::LinkUpdateMsg);

	uint8_t* stream;
	size_t size;
	std::tie(stream, size) = msg.encode();

	Message msg2;
	msg2.decode(stream, size);

	BOOST_TEST(msg2.payload_type()==Message::LinkUpdateMsg);
}

BOOST_AUTO_TEST_CASE( test_3, * utf::depends_on("Address_test/test_1"))
{
	Address src(0x0001ul, 0x1234ul);
	Address dst(0x0001ul, 0x2345ul);
	const char* m="hello,world!";
	Message msg(src, dst, Message::LinkUpdateMsg, m, strlen(m));

	Message msg2 = msg;

	BOOST_TEST(msg.payload() == msg2.payload());

	uint8_t* m2 = new uint8_t[8];
	Message msg3(src, dst, Message::LinkUpdateMsg, m2, strlen(m));
	BOOST_TEST(msg3.payload() == m2);

}

BOOST_AUTO_TEST_SUITE_END()