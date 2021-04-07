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
    Address dst;        //destination snode/enode address
	uint64_t mask;      //snode adderss mask
	int metric;         //mectric of this route path
    Address next_hop;   //next snode/enode for forwarding
	port_ptr port;      //the router's port for input/output
};

class RouteTable
{
public:
    port_ptr routing(const Address&	dst);
   
    void add(const Address &dst,
             int metric,
             const Address& next_hop, 
             const snode::port_ptr &port
    );

    std::vector<RouteItem> getAllItems()const;

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
