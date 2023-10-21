//sn_snode_impl.cpp

#include "sn_snode_impl.h"

namespace snode {

SnodeImpl::SnodeImpl(RouterImpl *router,
                     AddressManager* addressmgr,
                     TransportManager *transportmgr)
    :_router(router)
    ,_addressmgr(addressmgr)
    ,_transportmgr(transportmgr)
{
}

void SnodeImpl::setAddress(const Address& addr)
{
    _address = addr;
}

bool SnodeImpl::setupCommandTransport(
    const std::string& local_ip, int local_port
)
{
    _local_cmd_ip = local_ip;
    _local_cmd_port = local_port;
    return true;
}

void SnodeImpl::addNeighbor(const Neighbor& neib)
{
    UdpTransport* udp = _transportmgr->getUdpTransport(
            TransEndpoint{_local_forward_ip, neib.lport}
    );
    udp->remote_ep(neib.rendpoint);

    port_ptr port = std::make_shared<Port>();
    port->setTransport(udp);
    _router->addPort(neib.addr, port);
    
    _router->addRouting(neib.addr, 1, neib.addr);
    _neighbors.push_back(neib);
}

void SnodeImpl::addStaticRoute(
    const Address& dst, int metric, const Address& next_hop
)
{
    _router->addRouting(dst, metric, next_hop);
}

void SnodeImpl::addNeighbor(Address addr, port_ptr port, const TransEndpoint &remote_ep)
{
    port->transport()->remote_ep(remote_ep);
    _router->addPort(addr, port);
    
    _router->addRouting(addr, 1, addr);
    Neighbor neib{addr, port->transport()->local_ep().port, remote_ep};
    _neighbors.push_back(neib);
}

port_ptr SnodeImpl::newPort()
{
    UdpTransport* udp = _transportmgr->getUdpTransport(
            TransEndpoint{_local_forward_ip, 0}
    );

    port_ptr port = std::make_shared<Port>();
    port->setTransport(udp);

    return port;
}

Address SnodeImpl::allocAddress()
{
    return _addressmgr->allocAddress();
}

void SnodeImpl::releaseAddress(const Address &addr)
{
    _addressmgr->releaseAddress(addr);
}

}//namespace snode

