///sn_rip_routing.cpp


#include "sn_rip_routing.h"
#include "sn_rip_message.h"

namespace snode {

RIPRoutingMethod::RIPRoutingMethod()
{
}

void RIPRoutingMethod::start()
{
    int interval = getRandomNum({25, 35});
    _periodic_timer.start(interval, std::bind(&RIPRoutingMethod::onAdvertisingRoutingInfo, this));
}

void RIPRoutingMethod::onRecvMsg(const uint8_t *msg_buf, size_t size)
{
    RIPMessage msg;
    try{
        rip_message_parse(msg, msg_buf, size);

    }catch(const std::exception& e){
        //log e.what();
    }
}

void RIPRoutingMethod::updateRouting(const Address& from, const RoutingInfo& info)
{
    if(_route_table == nullptr){
        return;
    }

    for(auto item : info.items){
        auto ritem = _route_table->find_dst(item.dst);
        if(ritem != nullptr){
            auto port = _route_table->routing(from);
            _route_table->add(item.dst, item.metric+1, from, port);
        }
        else{
            if(ritem->next_hop = from){
                if(ritem->metric != item.metric){
                   ritem->metric = item.metric + 1;
                }
            }
            else{
                if(item.metric < ritem->metric){
                    ritem->metric = item.metric + 1;
                    ritem->next_hop = from;
                    ritem->port = _route_table->routing(from);
                }
            }
        }
    }
}


void RIPRoutingMethod::onAdvertisingRoutingInfo()
{
    if(_route_table == nullptr){
        return;
    }

    std::vector<RoutingInfo> infos;
    for(const auto& n : _neighbors){
        RoutingInfo info;
        auto ritems = _route_table->getAllItems();
        for(const auto& ritem : ritems){
            //Do not send the item to the neighor which is next hop address
            if(ritem.next_hop != n.addr){
                info.items.push_back({ritem.dst, ritem.mask, ritem.metric});
            }
        }
        infos.push_back(info);
    }

    if(_shared_cb != nullptr)
        _shared_cb(infos);
}

}//namespace snode

