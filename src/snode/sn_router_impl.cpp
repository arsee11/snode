///sn_router_impl.cpp


#include "sn_router_impl.h"

using namespace std::placeholders;

namespace snode {

RouterImpl::RouterImpl(RoutingMethod *rm)
    :_routing_method(rm)
{
    _routing_method->route_table(&_route_table);
    _routing_method->listenMessageReady(std::bind(&RouterImpl::onLinkUpdateMessageReady, this, _1, _2, _3));
    _routing_method->listenRequiredPort(std::bind(&RouterImpl::requirePort, this, _1));
}

void RouterImpl::setThreadingScope(ThreadScopePolling *thr)
{
    assert(thr != nullptr);
    assert(_routing_method != nullptr);
    _thrscope = thr;
    _routing_method->setThreadingScope(thr);
    _routing_method->start();
}

void RouterImpl::stop()
{
    _routing_method->stop();
}

void RouterImpl::addRouting(const Address &dst, int metric, const Address &next_hop)
{
    auto port = findPort(next_hop);
    if(port != nullptr )
        _route_table.add(dst, metric, next_hop, port);
    else
        throw std::runtime_error("port not found!");
}

void RouterImpl::updateRouting(const Address &dst, int metric, const Address &next_hop)
{
    auto item = _route_table.find_best_by_dst(dst);
    if(item == nullptr){
        addRouting(dst, metric, next_hop);
    }
    else{
        item->dst = dst;
        item->metric = metric;
        item->next_hop = next_hop;
    }
}

void RouterImpl::addNeighbor(const Address &n)
{
    if(_routing_method != nullptr){
        _routing_method->addNeighbor(n);
    }
}

// void RouterImpl::addDirectLink(const Address &next_hop, const port_ptr &port)
// {
//     addPort(next_hop, port);
//     Address dst_sn(next_hop.sn(), 0u);
//     addRouting(dst_sn, 1, next_hop, port);
//     _routing_method->addDirectLink(next_hop);
// }

void RouterImpl::onLinkUpdate(const Message& msg)
{
    //std::cout<<__FUNCTION__<<" from "<<msg.src()<<" size "<<msg.size()<<std::endl;

    if(_routing_method == nullptr){
        //log 
        return;
    }

    _routing_method->onRecvMsg(msg.src(), msg.payload(), msg.payload_size());
}

void RouterImpl::onLinkUpdateMessageReady(const Address &to, const uint8_t *msg_buf, size_t size)
{
    //std::cout<<__FUNCTION__<<" to "<<to<<" size "<<size<<std::endl;
    std::unique_ptr<Message> msg( Message::create_link_update_message(my_address(), to, msg_buf, size) );
    port_ptr port = _route_table.routing(to);
    if(port != nullptr){
        port->output(*msg);
    }
}

void RouterImpl::requirePort(routeitem_ptr item)
{
    item->port = findPort(item->next_hop);
    if(item->port == nullptr){
        std::cout<<"No port found for next_hop:"<< item->next_hop<<std::endl;
        //log
    }
}

}//namespace snode


