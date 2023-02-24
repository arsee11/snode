///sn_rip_routing.h

#ifndef SN_RIP_ROUTING_H
#define SN_RIP_ROUTING_H

#include "sn_routing.h"
#include "sn_route_table.h"

namespace snode {

class RIPRoutingMethod : public RoutingMethod
{
public:
    RIPRoutingMethod(RouteTable* rt)
        :_route_table(rt)
    {
    }

    ///multi thread access is not safe
    void updateRouting(const Address& from, const RoutingInfo& info);

    void shareRouting();
private:
    RouteTable* _route_table;
};

}//namespace snode

#endif /*SN_RIP_ROUTING_H*/
