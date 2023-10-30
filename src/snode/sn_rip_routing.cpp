///sn_rip_routing.cpp


#include "sn_rip_routing.h"
#include "sn_rip_message.h"
#include <vector>
#include <random>

namespace snode {

/// @brief generate a randon num in [from, to] area
static int getRandomNum(int from, int to)
{
    std::random_device r;
    std::minstd_rand engine(r());
    std::uniform_int_distribution<int> dist(from, to);
    int mean = dist(engine);
    return mean;
}

RIPRoutingMethod::RIPRoutingMethod()
{
}

void RIPRoutingMethod::setThreadingScope(ThreadScopePolling *thr)
{
    _thrscope = thr;
}

void RIPRoutingMethod::start()
{
    assert(_thrscope != nullptr);
    //int interval = getRandomNum(25, 35);
    int interval = getRandomNum(5, 8);
    _periodic_timer.reset(
        Timer::start(interval*1000, 
            std::bind(&RIPRoutingMethod::onAdvertisingRoutingInfo, this), _thrscope->poller())
    );
}

void RIPRoutingMethod::stop()
{
    _periodic_timer->stop();
}

void RIPRoutingMethod::addNeighbor(const Address& n)
{
    auto it = std::find(_neighbors.begin(), _neighbors.end(), n);
    if(it == _neighbors.end()){
        _neighbors.push_back(n);
    }
}

void RIPRoutingMethod::removeNeighbor(const Address& n)
{
    auto it = std::find(_neighbors.begin(), _neighbors.end(), n);
    if(it != _neighbors.end()){
        _neighbors.erase(it);
    }
}

void RIPRoutingMethod::onRecvMsg(const Address& from, const uint8_t *msg_buf, size_t size)
{
    RIPMessage msg = RIPMessage::parse(msg_buf, size);
    if(msg.command == RIPMessage::InvalidCommand){
        //log<<"Invalide RIP message received."
        return;
    }

    if(msg.command == RIPMessage::RequestCommand){
        //log<<"requst RIP message received."
        if(_route_table == nullptr){
            return;
        }

        shareRoutingInfo(from);
        return;
    }

    if(msg.command == RIPMessage::ResponseCommand){
        //log<<"response RIP message received."
        RoutingInfo info;
        for(auto f : msg.fields){
            RoutingInfo::Item item;
            item.dst = f.address;
            item.metric = f.metric;
            info.items.push_back(item);
        }
        std::cout<<"recv flesh routing info from "<<from<<":"<<std::endl;
        std::cout<<info.items<<std::endl;
        updateRouting(from, info);
        return;
    }


}

void RIPRoutingMethod::updateRouting(const Address& from, const RoutingInfo& info)
{
    if(_route_table == nullptr){
        return;
    }

    for(auto item : info.items){
        auto ritem = _route_table->find_best_by_dst(item.dst);
        if(ritem == nullptr){ //is a new item
            auto port = _route_table->routing(from);
            ritem = _route_table->add(item.dst, item.metric+1, from, port);
            if(_required_port_cb != nullptr && port == nullptr){
                _required_port_cb(ritem);
            }
        }
        else{// is an existed item
            if(ritem->next_hop == from){ //if next hop is "from", so update this item 
                if(ritem->metric != item.metric+1){
                   ritem->metric = item.metric + 1;
                }
            }
            else{//otherwise if it is a better one, so update it.
                if(item.metric+1 < ritem->metric){
                    ritem->metric = item.metric + 1;
                    ritem->next_hop = from;
                }
            }
        }
    }
}


void RIPRoutingMethod::onAdvertisingRoutingInfo()
{
    std::cout<<__FUNCTION__<<std::endl;
    if(_route_table == nullptr){
        return;
    }

    for(const auto& n : _neighbors){
         shareRoutingInfo(n);
    }
}

void RIPRoutingMethod::shareRoutingInfo(const Address& to)
{
    RIPMessage msg;
    msg.command = RIPMessage::ResponseCommand;
    auto ritems = _route_table->getAllItems();
    for(const auto& ritem : ritems){
        //Do not send the item to the neighor which is next hop address
        if(ritem->next_hop != to && ritem->dst.en() == 0){
            RIPMessage::field f;
            f.family = 0;
            f.address = ritem->dst.raw();
            f.metric = static_cast<uint16_t>(ritem->metric);
            msg.fields.push_back(f);
        }
    }
    msg.nfields = static_cast<uint8_t>(msg.fields.size());
    if(_shared_cb != nullptr && msg.nfields > 0){
        std::array<uint8_t, 1400> buf;
        size_t size = RIPMessage::serialize(msg, buf.data(), buf.size());
        _shared_cb(to, buf.data(), size);
    }
    std::cout<<"sharing routing info to "<<to<<" No. of items:"<<msg.fields.size()<<std::endl;
}

}//namespace snode

