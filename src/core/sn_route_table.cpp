///sn_route_table.cpp

#include "sn_route_table.h"


namespace snode{

port_ptr snode::RouteTable::routing(const Address &dst)
{
    ///first match full address
    auto ilist = _routes.find(dst);
    if( ilist == _routes.end() ){
        ///second if full address not match, then macth the sn address
        Address saddr(dst.sn(), 0ul);
        ilist = _routes.find(saddr);
        if( ilist == _routes.end() )
            return nullptr;
    }
    return getBestRoute(ilist->second);
}

void RouteTable::add(const Address &dst,
                    int metric,
                    const Address& next_hop, 
                    const snode::port_ptr &port
)
{
    //dst, mask, next_hop, port
    RouteItem item{dst, 0, metric, next_hop, port};
    auto ilist = _routes.find(dst);
    if( ilist == _routes.end() )
    {
        List list;
        list.push_back(item);
        _routes.insert({dst, list});
    }
    else
        ilist->second.push_back(item);
}

std::vector<RouteItem> RouteTable::getAllItems()const
{
    std::vector<RouteItem> items;
    for(auto& it: _routes){
        auto& l = it.second;
        for(auto& i : l){
            items.push_back(i); 
        }
    }

    return items;
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
