///sn_routing.h

#ifndef SN_ROUTING_H
#define SN_ROUTING_H

#include "core/sn_address.h"
#include "core/sn_route_table.h"
#include <vector>

namespace snode {

struct RoutingInfo
{
    struct Item{
        Address dst;        //destination snode/enode address
	    uint64_t mask;      //snode adderss mask
	    int metric;         //metric of this route path
    };

    std::vector<Item> items;
};



struct Neighbor
{
    Address addr;           //remote (the Neighbor)'s Address
    uint16_t lport;         //local endpoint port
    TransEndpoint rendpoint;//remote(the Neighbor)'s endpoint
};
inline bool operator==(const Neighbor& lhs, const Neighbor& rhs){
    return ( lhs.addr == rhs.addr );
}



class RoutingMethod
{
public:
    using message_cb = std::function<void(const Address& to, const uint8_t *msg_buf, size_t size)>;
    using required_port_cb = std::function<void(routeitem_ptr item)>;
    
public:
    virtual ~RoutingMethod(){}
    virtual void addNeighbor(const Neighbor& n)=0;
    virtual void removeNeighbor(const Neighbor& n)=0;
    virtual void onRecvMsg(const Address& from, const uint8_t* msg, size_t size)=0;
    virtual void listenMessageReady(const message_cb& cb)=0;
    virtual void listenRequiredPort(const required_port_cb& cb)=0;
    virtual void route_table(RouteTable* rt)=0;
    virtual void updateRouting(const Address& from, const RoutingInfo& info)=0;

};

}//namespace snode

#endif/*SN_ROUTING_H*/
