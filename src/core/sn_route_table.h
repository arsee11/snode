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
	int metric;         //metric of this route path
    Address next_hop;   //next snode/enode for forwarding
	port_ptr port;      //the router's port for input/output
};

using routeitem_ptr = std::shared_ptr<RouteItem>;

class RouteTable
{
public:
    port_ptr routing(const Address&	dst)const;
    routeitem_ptr find_best_by_dst(const Address& dst)const;

    void add(const Address &dst,
             int metric,
             const Address& next_hop, 
             const snode::port_ptr &port
    );

    std::vector<routeitem_ptr> getAllItems()const;

private:
    ///|dst|RouteItem|
    ///|1  |item1     |
    ///|   |item2     |
    ///|2  |item1     |
    ///|   |item2     |
    using List = std::list<routeitem_ptr>;
    ///@key dst snode address
    ///@value RouteItem
    using Table = std::unordered_map<Address, List, AddressHash>;
    Table _routes;

private:
    List find_dst(const Address& dst)const;
    routeitem_ptr getBestItem(const List& l)const;
    port_ptr getBestRoute(const List& list)const;

};

}

#endif /*SN_ROUTER_TABLE_H*/
