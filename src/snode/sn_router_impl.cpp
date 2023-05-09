///sn_router_impl.cpp


#include "sn_router_impl.h"

namespace snode {

RouterImpl::RouterImpl(RoutingMethod *rm)
    :_routing_method(rm)
{

    _routing_method->listenNewEntries(std::bind(&RouterImpl::onNewEntries, this, _1));
    _routing_method->listenUpdateEntries(std::bind(&RouterImpl::onUpdateEntries, this, _1));
    _routing_method->listenDisableEntries(std::bind(&RouterImpl::onDisableEntries, this, _1));
    _routing_method->listenEnableEntries(std::bind(&RouterImpl::onEnableEntries, this, _1));
    _routing_method->listenDelEntries(std::bind(&RouterImpl::onDelEntries, this, _1));
}

bool RouterImpl::start()
{

}

void RouterImpl::addRouting(const Address &dst, int metric, const Address &next_hop, const port_ptr &port)
{

    auto i = std::find(_ports.begin(), _ports.end(), port);
    if(i != _ports.end() )
        _route_table.add(dst, metric, next_hop, port);
    else
        throw std::runtime_error("port not found!");
}

void RouterImpl::addDirectLink(const Address &next_hop, const port_ptr &port)
{
    addPort(next_hop, port);
    Address dst_sn(next_hop.sn(), 0u);
    addRouting(dst_sn, 1, next_hop, port);
    _routing_method->addDirectLink(next_hop);
}

void RouterImpl::onLinkUpdate(const Message& msg)
{
    if(_routing_method == nullptr){
        //log 
        return;
    }

    _routing_method->onRecvMsg(msg.payload(), msg.payload_size());
}

void RouterImpl::onNewEntries(const RoutingInfo& info)
{
    for(const auto& item : info.items){
        port_ptr port = findPort(item.next_hop);
        if(port == nullptr){
            //log no port was found for this direct-link:info.next_hop.
            return;
        }
        Address dst_sn(item.dst.sn(), 0u);
        addRouting(dst_sn, item.metric, item.next_hop, port);
    }
}

void RouterImpl::onUpdateEntries(const RoutingInfo& info)
{
    for(const auto& item : info.items){
        _route_table.updateMetric(item.dst, item.next_hop, item.metric);
    }
}

void RouterImpl::onDisableEntries(const RoutingInfo& info)
{
    for(const auto& item : info.items){
        _route_table.disableItem(item.dst, item.next_hop);
    }
}

void RouterImpl::onEnableEntries(const RoutingInfo& info)
{
    for(const auto& item : info.items){
        _route_table.enableItem(item.dst, item.next_hop);
    }
}

void RouterImpl::onDelEntries(const RoutingInfo& info)
{
    for(const auto& item : info.items){
        _route_table.delItem(item.dst, item.next_hop);
    }
}



}//namespace snode


