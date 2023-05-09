///sn_rip_routing.h

#ifndef SN_RIP_ROUTING_H
#define SN_RIP_ROUTING_H

#include "core/sn_routing.h"
#include "core/sn_route_table.h"
#include "sn_neighbor.h"

namespace snode {

class RIPRoutingMethod : public RoutingMethod
{
public:
    RIPRoutingMethod();
    void start();
    void stop();
    void addNeighbor();
    void removeNeighbor();

    // RoutingMethod interface
public:
    void onRecvMsg(const uint8_t *msg_buf, size_t size);

private:
    void onAdvertisingRoutingInfo();
    void updateRouting(const Address& from, const RoutingInfo& info);

    Timer _periodic_timer;
    Timer _expiration_timer;
    Timer _garbage_collection_timer;
    RouteTable* _route_table=nullptr;
    std::list<Neighbor> _neighbors;


};

}//namespace snode

#endif /*SN_RIP_ROUTING_H*/
