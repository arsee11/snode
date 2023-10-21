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

bool RouterImpl::start()
{
    return true;
}

void RouterImpl::addRouting(const Address &dst, int metric, const Address &next_hop)
{
    auto port = findPort(next_hop);
    if(port != nullptr )
        _route_table.add(dst, metric, next_hop, port);
    else
        throw std::runtime_error("port not found!");
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
    if(_routing_method == nullptr){
        //log 
        return;
    }

    _routing_method->onRecvMsg(msg.src(), msg.payload(), msg.payload_size());
}

void RouterImpl::onLinkUpdateMessageReady(const Address &to, const uint8_t *msg_buf, size_t size)
{
    Message msg = Message::create_link_update_message(Address::any(), to, msg_buf, size);
    port_ptr port = _route_table.routing(to);
    if(port != nullptr){
        port->output(msg);
    }
}

void RouterImpl::requirePort(routeitem_ptr item)
{
    item->port = findPort(item->next_hop);
}

}//namespace snode


