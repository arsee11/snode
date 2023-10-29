///sn_route_table.cpp

#include "sn_route_table.h"


namespace snode{

port_ptr RouteTable::routing(const Address &dst)const
{  
    return getBestRoute(find_dst(dst));
}

routeitem_ptr RouteTable::find_best_by_dst(const Address &dst) const
{
    const List& l = find_dst(dst);
    if(l.empty()){
        return nullptr;
    }

    return getBestItem(l);
}

RouteTable::List RouteTable::find_dst(const Address &dst) const
{
    ///first match full address
    auto ilist = _routes.find(dst);
    if( ilist == _routes.end() ){
        ///second if full address not match, then macth the sn address
        Address saddr(dst.sn(), 0ul);
        ilist = _routes.find(saddr);
        if( ilist == _routes.end() )
            return List();
    }
    return ilist->second;
}

routeitem_ptr RouteTable::add(const Address &dst,
                    int metric,
                    const Address& next_hop, 
                    const snode::port_ptr &port
)
{
    //dst, mask, next_hop, port
    routeitem_ptr item( new RouteItem{dst, 0, metric, next_hop, port});
    auto ilist = _routes.find(dst);
    if( ilist == _routes.end() )
    {
        List list;
        list.push_back(item);
        _routes.insert({dst, list});
    }
    else
        ilist->second.push_back(item);

    return item;
}

std::vector<routeitem_ptr> RouteTable::getAllItems()const
{
    std::vector<routeitem_ptr> items;
    for(auto& it: _routes){
        auto& l = it.second;
        for(auto& i : l){
            items.push_back(i); 
        }
    }

    return items;
}

port_ptr RouteTable::getBestRoute(const List& list)const
{
    if(list.size() == 0){
        return nullptr;
    }

    routeitem_ptr item = getBestItem(list);
    return item->port;
}

routeitem_ptr RouteTable::getBestItem(const List& l)const
{
    auto first = l.begin();

    for(auto it=++l.begin(); it!=l.end(); ++it){
        if( (*it)->metric < (*first)->metric ){
            first = it;
        }
    }
    return *first;
}

}//namespace snode
