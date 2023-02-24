///sn_rip_routing.cpp


#include "sn_rip_routing.h"

namespace snode {

///not multi thread access
void RIPRoutingMethod::updateRouting(const Address& from, const RoutingInfo& info)
{
//    if(_route_table == nullptr){
//        return;
//    }

//    for(auto item : info.items){
//        auto ritem = _route_table->find_dst(item.dst);
//        if(ritem != nullptr){
//            auto port = _route_table->routing(from);
//            _route_table->add(item.dst, item.metric+1, from, port);
//        }
//        else{
//            if(ritem->next_hop = from){
//                if(ritem->metric != item.metric){
//                   ritem->metric = item.metric + 1;
//                }
//            }
//            else{
//                if(item.metric < ritem->metric){
//                    ritem->metric = item.metric + 1;
//                    ritem->next_hop = from;
//                    ritem->port = _route_table->routing(from);
//                }
//            }
//        }
//    }
}

void RIPRoutingMethod::shareRouting()
{
//    if(_route_table == nullptr){
//        return;
//    }

//    std::vector<RoutingInfo> infos;
//    for(const auto& n : _neighbours){
//        RoutingInfo info{n};
//        infos.push_back(info);
//        auto ritems = _route_table->getAllItems();
//        for(const auto& ritem : ritems){
//            if(ritem.next_hop != n.addr){
//                info.items.push_back({ritem.dst, ritem.mask, ritem.metric});
//            }
//        }
//    }

//    if(_shared_cb != nullptr)
//        _shared_cb(infos);
}

}//namespace snode

