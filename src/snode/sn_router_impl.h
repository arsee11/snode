///sn_router_impl.h

#ifndef SN_ROUTER_IMPL_H
#define SN_ROUTER_IMPL_H

#include "core/sn_router.h"
#include "core/sn_rip_routing.h"
#include "core/sn_route_table.h"

namespace snode {

using RouterImpl=Router<RIPRoutingMethod, RouteTable>;

}//namespace snode


#endif /*SN_ROUTER_IMPL_H*/
