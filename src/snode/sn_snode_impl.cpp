//sn_snode_impl.cpp

#include "sn_snode_impl.h"

namespace snode {

SnodeImpl::SnodeImpl(RouterImpl *router)
    :_router(router)
{
}

void SnodeImpl::setAddress(const Address& addr)
{
    _address = addr;
    _addressmgr = AddressManager(addr.sn());
}

bool SnodeImpl::setupCommandTransport(
    const std::string& local_ip, int local_port
)
{
    _local_cmd_ip = local_ip;
    _local_cmd_port = local_port;
}

void SnodeImpl::setNeighbor(const Neighbor& neib)
{
    UdpTransport* udp = _transportmgr.getUdpTransport(
            TransEndpoint{_local_forward_ip, neib.lport}
    );
    udp->remote_ep(neib.rendpoint);

    port_ptr port = std::make_shared<Port>();
    port->setTransport(udp);
    _router->addPort(port);
    
    Address neib_sn(neib.addr.sn(), 0u);
    _router->addRouting(neib_sn, 1, neib.addr, port);
}

void SnodeImpl::addStaticRoute(
    const Address& dst, int metric, const Address& next_hop
)
{
    auto port = _router->route_table().routing(next_hop);
    if(port == nullptr){
        std::cout<<"next_top["<<next_hop.sn()<<","<<next_hop.en()<<"] not reachable\n";
        return;
    }

    _router->addRouting(dst, metric, next_hop, port);
}

Address SnodeImpl::directlyRegister(Transport* trans)
{
    Address addr = _addressmgr.allocAddress();
	port_ptr port = std::make_shared<Port>();
    port->setTransport(trans);
    _router->addPort(port);
    _router->addRouting(addr, 1, addr, port);
}

}//namespace snode

