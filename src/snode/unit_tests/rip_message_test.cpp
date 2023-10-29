
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include "snode/sn_rip_message.h"
#include <arpa/inet.h>

using namespace snode;
using namespace std;

BOOST_AUTO_TEST_SUITE(rip_message)

BOOST_AUTO_TEST_CASE(test1)
{
    int32_t n=10086;
    uint8_t data[8];

    write_bytes<int32_t, 4>(data, n);
    int32_t m = *((int32_t*)data);
    m = ntohl(m);
    BOOST_TEST(n == m);
    m = read_bytes<int32_t, 4>(data);
    BOOST_TEST(n == m);

    write_bytes<uint8_t, 1>(data, 233);
    uint8_t r = read_bytes<uint8_t, 1>(data);
    BOOST_TEST(r == 233);
}

BOOST_AUTO_TEST_CASE(test2)
{
    RIPMessage msg;
    BOOST_TEST(msg.command == RIPMessage::InvalidCommand);
    msg.command = RIPMessage::RequestCommand;

    BOOST_TEST(msg.size() == 3);

    msg.fields.push_back({0, 1000u, 1});
    msg.nfields = 1;
    BOOST_TEST(msg.size() == msg.header_size()+RIPMessage::field_size);

    msg.fields.push_back({0, 1001u, 3});
    msg.fields.push_back({0, 1002u, 4});
    msg.nfields = 3;

    uint8_t data[64];
    int n = RIPMessage::serialize(msg, data, 24);
    BOOST_TEST(n == 0);
    n = RIPMessage::serialize(msg, data, 128);
    BOOST_TEST(n == msg.size());

    auto msg2 = RIPMessage::parse(data, n);

    BOOST_TEST_REQUIRE(msg2.command == msg.command);
    BOOST_TEST_REQUIRE(msg2.version == msg.version);
    BOOST_TEST_REQUIRE(msg2.nfields == msg.nfields);
    BOOST_TEST_REQUIRE(msg2.nfields == msg2.fields.size());

    for(int i=0; i<msg2.nfields; i++){
      BOOST_TEST(msg2.fields[i].family == msg.fields[i].family);
      BOOST_TEST(msg2.fields[i].address == msg.fields[i].address);
      BOOST_TEST(msg2.fields[i].metric == msg.fields[i].metric);
    }

}

BOOST_AUTO_TEST_SUITE_END()