///sn_router_impl.h

#ifndef SN_ROUTER_IMPL_H
#define SN_ROUTER_IMPL_H

#include "core/sn_router.h"
#include "core/sn_routing.h"
#include "core/sn_route_table.h"

namespace snode {

class  RouterImpl : public RouterT<RouteTable>
{
public:
    //take onwership of @rm
    RouterImpl(RoutingMethod* rm);
    void setThreadingScope(ThreadScopePolling* thr)override;

    ///@brief add static route item
    ///@param dst dst snode/node address
    ///@param metric mectric of this route path
    ///@param next_hop next snode for forwarding
    void addRouting(const Address& dst, int metric,
                    const Address& next_hop);

    ///@brief update static route item
    ///@param dst dst snode/node address
    ///@param metric mectric of this route path
    ///@param next_hop next snode for forwarding
    void updateRouting(const Address& dst, int metric,
                    const Address& next_hop);

    void addNeighbor(const Address& n)override;

    void my_address(const Address& addr){
        _my_address = addr;
    }

    Address my_address()const{
        return _my_address;
    }

protected:
    void onLinkUpdate(const Message& msg)override;
    void onLinkUpdateMessageReady(const Address& to, const uint8_t *msg_buf, size_t size);
    void requirePort(routeitem_ptr item);

protected:
    std::unique_ptr<RoutingMethod> _routing_method;
    Address _my_address;
};

}//namespace snode


#endif /*SN_ROUTER_IMPL_H*/
