///sn_rip_routing.h

#ifndef SN_RIP_ROUTING_H
#define SN_RIP_ROUTING_H

#include "core/sn_routing.h"
#include "core/sn_route_table.h"
#include "core/sn_timer.h"
#include "sn_neighbor.h"

namespace snode {

class RIPRoutingMethod : public RoutingMethod
{

public:
    RIPRoutingMethod();
    void start();
    void stop();

    // RoutingMethod interface
public:
    void addNeighbor(const Neighbor& n)override;
    void removeNeighbor(const Neighbor& n)override;
    void onRecvMsg(const Address& from, const uint8_t *msg_buf, size_t size)override;
    void listenMessageReady(const message_cb& cb)override{ _shared_cb = cb; }
    void listenRequiredPort(const required_port_cb& cb)override{ _required_port_cb = cb; }
    void route_table(RouteTable* rt)override{ _route_table = rt; }
    void updateRouting(const Address& from, const RoutingInfo& info)override;

private:
    void onAdvertisingRoutingInfo();
    void shareRoutingInfo(const Address& to);

    Timer _periodic_timer;
    Timer _expiration_timer;
    Timer _garbage_collection_timer;
    RouteTable* _route_table=nullptr;
    std::list<Neighbor> _neighbors;
    message_cb _shared_cb;
    required_port_cb _required_port_cb; 

};

}//namespace snode

#endif /*SN_RIP_ROUTING_H*/
