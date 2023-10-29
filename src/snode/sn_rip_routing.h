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
    void setThreadingScope(ThreadScopePolling* thr);

    // RoutingMethod interface
public:
    void addNeighbor(const Address& n)override;
    void removeNeighbor(const Address& n)override;
    void onRecvMsg(const Address& from, const uint8_t *msg_buf, size_t size)override;
    void listenMessageReady(const message_cb& cb)override{ _shared_cb = cb; }
    void listenRequiredPort(const required_port_cb& cb)override{ _required_port_cb = cb; }
    void route_table(RouteTable* rt)override{ _route_table = rt; }
    void updateRouting(const Address& from, const RoutingInfo& info)override;

private:
    void onAdvertisingRoutingInfo();
    void shareRoutingInfo(const Address& to);
    using timer_ptr = std::unique_ptr<Timer>;
    timer_ptr _periodic_timer;
    timer_ptr _expiration_timer;
    timer_ptr _garbage_collection_timer;
    RouteTable* _route_table=nullptr;
    std::list<Address> _neighbors;
    message_cb _shared_cb;
    required_port_cb _required_port_cb; 
    ThreadScopePolling* _thrscope=nullptr;
};

}//namespace snode

#endif /*SN_RIP_ROUTING_H*/
