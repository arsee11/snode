///sn_route_table.cpp

#include "sn_route_table.h"


snode::port_ptr snode::RouteTable::routing(const Address &dst)
{
    Address saddr(dst.sn(), 0ul);
    auto itable = _routes.find(saddr);
    if( itable == _routes.end() )
        return nullptr;

    auto table = itable->second;
    if(table.size() > 1)
    {
        auto item = table.find(dst);
        if(item != table.end())
            return item->second.port;
    }
    else if(table.size() == 1)
        return table.begin()->second.port;


    return nullptr;
}

void snode::RouteTable::add(const Address &dst_snode
                                   ,const Address &next_hop
                                   ,int metric
                                   ,const snode::port_ptr &port)
{
    assert(dst_snode.en() == 0);
    RouteItem item{dst_snode, 0, next_hop, metric, port};
    auto itable = _routes.find(dst_snode);
    if( itable == _routes.end() )
    {
        Table table;
        table.insert({next_hop, item});
        _routes.insert({dst_snode, table});
    }
    else
        itable->second.insert({next_hop, item});
}
