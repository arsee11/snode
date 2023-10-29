
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include "snode/sn_rip_routing.h"
#include "snode/sn_rip_message.h"
#include "snode/sn_router_impl.h"

using namespace snode;
using namespace std;

BOOST_AUTO_TEST_SUITE(rip_routing)

std::ostream& operator<<(std::ostream& os, const Address& addr){
	os<<"["<<addr.sn()<<"."<<addr.sn()<<"]";
	return os;
}

bool check_items(const Address& addr, 
  const std::vector<RoutingInfo::Item>& src, const std::vector<routeitem_ptr>& dst)
{
  for(auto i : src){
    auto j = std::find_if(dst.begin(), dst.end(), [&i](auto item){
      return item->dst == i.dst;
    });
    if(j == dst.end()){
      return false;
    }

    if( (*j)->next_hop != addr){
      BOOST_TEST_MESSAGE("next hop diff:{"<<((*j)->next_hop.sn())<<" vs "<<(addr.sn())<<"}");
      return false;
    }
    if( (*j)->metric != i.metric +1 ){
      BOOST_TEST_MESSAGE("metric diff:{"<<((*j)->metric)<<" vs "<<(i.metric +1)<<"}");
      return false;
    }
  }
  return true;
}

bool check_items(const Address& next_hop, const Address& dst_addr, int metric, 
  const std::vector<routeitem_ptr>& items)
{
  auto j = std::find_if(items.begin(), items.end(), [&dst_addr](auto item){
    return item->dst == dst_addr;
  });
  if(j == items.end()){
    return false;
  }

  if( (*j)->next_hop != next_hop){
      BOOST_TEST_MESSAGE("next hop diff:{"<<((*j)->next_hop.sn())<<" vs "<<(next_hop.sn())<<"}");
      return false;
    }
    if( (*j)->metric != metric ){
      BOOST_TEST_MESSAGE("metric diff:{"<<((*j)->metric)<<" vs "<<metric<<"}");
      return false;
    }
  return true;
}

BOOST_AUTO_TEST_CASE(test1)
{
  RouteTable rt;
  RIPRoutingMethod rtm;
  rtm.route_table(&rt);

  Address addr1(1u,1u);
  Address addr2(2u,1u);

  RoutingInfo info1;
  info1.items.push_back({{1000u, 2u}, 0, 1});
  info1.items.push_back({{1001u, 2u}, 0, 3});
  info1.items.push_back({{1002u, 2u}, 0, 4});

  rtm.updateRouting(addr1, info1);
  auto items = rt.getAllItems();
  BOOST_TEST_REQUIRE(items.size() == 3);
  BOOST_TEST(check_items(addr1, info1.items, items));
  BOOST_TEST_INFO(" for info1");

  RoutingInfo info2;
  info2.items.push_back({{1000u, 2u}, 0, 1});
  rtm.updateRouting(addr1, info2);
  BOOST_TEST_REQUIRE(rt.getAllItems().size() == 3);
  BOOST_TEST_INFO(" for info2");

  RoutingInfo info3;
  info3.items.push_back({{1004u, 2u}, 0, 1});
  rtm.updateRouting(addr1, info3);
  items = rt.getAllItems();
  BOOST_TEST_REQUIRE(items.size() == 4);
  BOOST_TEST(check_items(addr1, info3.items, items));
  BOOST_TEST_INFO(" for info3");

  RoutingInfo info4;
  info4.items.push_back({{1000u, 2u}, 0, 2});
  rtm.updateRouting(addr1, info4);
  items = rt.getAllItems();
  BOOST_TEST_REQUIRE(items.size() == 4);
  BOOST_TEST(check_items(addr1, info4.items, items));
  BOOST_TEST_INFO(" for info4");

  RoutingInfo info5;
  info5.items.push_back({{1000u, 2u}, 0, 4});
  rtm.updateRouting(addr2, info5);
  items = rt.getAllItems();
  BOOST_TEST(check_items(addr1, Address(1000u, 2), 3, items));
  BOOST_TEST_INFO(" for info5");

  RoutingInfo info6;
  info6.items.push_back({{1000u, 2u}, 0, 1});
  rtm.updateRouting(addr2, info6);
  items = rt.getAllItems();
  BOOST_TEST_REQUIRE(items.size() == 4);
  BOOST_TEST(check_items(addr2, info6.items, items));
  BOOST_TEST_INFO(" for info6");

}

BOOST_AUTO_TEST_CASE(test2)
{
  RouteTable rt;
  RIPRoutingMethod rtm;
  rtm.route_table(&rt);

  Address addr1(1u,1u);
  RIPMessage msg;
  msg.command = RIPMessage::ResponseCommand;
  msg.fields.push_back({0, Address(1005u, 2u).raw(), 4});
  msg.nfields = 1;
  uint8_t data[64];
  int n = RIPMessage::serialize(msg, data, 64);  
  rtm.onRecvMsg(addr1, data, n);

  auto items = rt.getAllItems();
  BOOST_TEST_REQUIRE(items.size() == 1);
  BOOST_TEST((items[0]->next_hop == addr1));
  BOOST_TEST(items[0]->dst.sn() == 1005u);
  BOOST_TEST(items[0]->metric == 5);


  // Neighbor n1{Address(1u,1u), 10086, {"192.168.1.1", 10010}};
  // rtm.addNeighbor(n1);

}

BOOST_AUTO_TEST_CASE(test3)
{
  EventQueueEpoll eq;
  ThreadScopePolling thr(&eq);
  RIPRoutingMethod* rtm = new RIPRoutingMethod;
  RouterImpl router(rtm);
  router.setThreadingScope(&thr);

  Address addr1(1u,1u);
  port_ptr port( new Port);
  router.addPort(addr1, port);
  
  Address addr2(2u, 0u);
  RoutingInfo info;
  info.items.push_back({addr2, 0, 1});
  rtm->updateRouting(addr1,  info);

  auto port2 = router.route_table().routing(addr2);
  BOOST_TEST(port2 == port);

  thr.stop();

}

BOOST_AUTO_TEST_SUITE_END()