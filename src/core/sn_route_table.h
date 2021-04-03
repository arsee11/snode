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
	int metric;
	port_ptr port;
};

class RouteTable
{
public:
    port_ptr routing(const Address&	dst);

    ///@param dst_snode dst snode address, Address::en()==0
    ///@param mectric mectric of this route path
    ///@param port the router's port for input/output
    void  add(const Address& dst_snode
                     ,int metric
                     ,const port_ptr& port);


private:
    ///|dst|RouteItem|
    ///|1  |item1     |
    ///|   |item2     |
    ///|2  |item1     |
    ///|   |item2     |
    using List = std::list<RouteItem>;
    ///@key dst snode address
    ///@value RouteItem
    using Table = std::unordered_map<Address, List, AddressHash>;
    Table _routes;

private:
     port_ptr getBestRoute(const List& list);

};

}

#endif /*SN_ROUTER_TABLE_H*/
