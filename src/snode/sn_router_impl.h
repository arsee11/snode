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
    RouterImpl(RoutingMethod* rm);
    bool start();

    ///@brief add static route item
    ///@param dst dst snode/node address
    ///@param metric mectric of this route path
    ///@param next_hop next snode for forwarding
    void addRouting(const Address& dst, int metric,
                    const Address& next_hop);

    void addDirectLink(const Address& next_hop, const port_ptr& port)override;


protected:
    void onLinkUpdate(const Message& msg)override;
    void onLinkUpdateMessageReady(const Address& to, const uint8_t *msg_buf, size_t size);
    void requirePort(routeitem_ptr item);

protected:
    RoutingMethod* _routing_method;
};

}//namespace snode


#endif /*SN_ROUTER_IMPL_H*/
