///sn_route_table.cpp

#include "sn_route_table.h"


namespace snode{

port_ptr snode::RouteTable::routing(const Address &dst)
{
    Address saddr(dst.sn(), 0ul);
    auto ilist = _routes.find(saddr);
    if( ilist == _routes.end() )
        return nullptr;

     return getBestRoute(ilist->second);
}

void RouteTable::add(const Address &dst_snode
                                   ,int metric
                                   ,const snode::port_ptr &port)
{
    assert(dst_snode.en() != 0);
    RouteItem item{dst_snode, 0, metric, port};
    auto ilist = _routes.find(dst_snode);
    if( ilist == _routes.end() )
    {
        List list;
        list.push_back(item);
        _routes.insert({dst_snode, list});
    }
    else
        ilist->second.push_back(item);
}

port_ptr RouteTable::getBestRoute(const List& list)
{
    if(list.size() == 0){
        return nullptr;
    }

    auto first = list.begin();

    for(auto it=++list.begin(); it!=list.end(); ++it){
        if(it->metric < first->metric ){
            first = it;
        }
    }

    return first->port;
}


}//namespace snode
