///sn_router_table.h

#ifndef SN_ROUTER_TABLE_H
#define SN_ROUTER_TABLE_H

#include "sn_port.h"
#include "sn_address.h"
#include <unordered_map>

namespace snode
{

struct RouteItem
{
    Address dst_snode_addr;
	uint64_t mask;
	Address next_hop;
	int metric;
	port_ptr port;
};

class RouteTable
{
public:
    port_ptr routing(const Address&	dst);

    ///@param dst_snode dst snode address, Address::en()==0
    ///@param next_hop next hop snode address
    ///@param mectric mectric of this route path
    ///@param port the router's port for input/output
    void  add(const Address& dst_snode
                     ,const Address& next_hop
                     ,int metric
                     ,const port_ptr& port);

private:
    ///|dst|next hop|RouteItem|
    ///|1  |1       |item     |
    ///|   |2       |item     |
    ///@key next hop address
    ///@value RouteItem
    using Table = std::unordered_map<Address, RouteItem, AddressHash>;
    ///@key dst snode address
    ///@value RouteItem
    using Table2 = std::unordered_map<Address, Table, AddressHash>;
    Table2 _routes;
};

}

#endif /*SN_ROUTER_TABLE_H*/
