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
    _address = addressmgr->root_address();
    _thrscope.reset(new ThreadScopePolling(&_event_eq));
    _thrscope->name("snode_scope");
    _router->setThreadingScope(_thrscope.get());
    _router->my_address(_address);
}

SnodeImpl::~SnodeImpl()
{
    _router->stop();
}

Address SnodeImpl::getAddress() const
{
    return _address;
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
    assert(_thrscope != nullptr);
    _thrscope->post([this, neib]{
        _router->addPort(neib.addr, neib.port);    
        _neighbors.insert({neib.addr, neib});
    });
}

void SnodeImpl::configNeighbor(const Address& neib_addr, const TransEndpoint& neib_port_ep)
{
    assert(_thrscope != nullptr);
    _thrscope->post([this, neib_addr, neib_port_ep]{
        auto neib = _neighbors.find(neib_addr);
        if(neib == _neighbors.end()){
            //loge()<<"not configtable neighbor addr"<<neib_addr;
            return;
        }
        
        auto port = _router->findPort(neib_addr);
        if(port == nullptr){
            //logi()<<"no port for neighbor addr"<<neib_addr<<" create one";
            port = newPort();
            _router->addPort(neib_addr, port);
            neib->second.port = port;
        }
        port->transport()->remote_ep(neib_port_ep);
        _router->updateRouting(Address(neib_addr.sn(), 0u), 1, neib_addr);
        _router->addNeighbor(neib_addr);
    });
}

NeighborMap SnodeImpl::getNeighbors() const
{
    assert(_thrscope != nullptr);
    return _thrscope->invoke([this]{
        NeighborMap ns;
        for(auto i : _neighbors){
            ns.push_back(i.second);
        }
        return ns;
    });

}

void SnodeImpl::addStaticRoute(
    const Address& dst, int metric, const Address& next_hop
)
{
    assert(_thrscope != nullptr);
    _thrscope->post([this, dst, metric, next_hop]{
        _router->addRouting(dst, metric, next_hop);
    });
}

void SnodeImpl::addEnode(Address addr, port_ptr port, const TransEndpoint &remote_ep)
{
    assert(_thrscope != nullptr);
    _thrscope->post([this, addr, port, remote_ep]{
        port->transport()->remote_ep(remote_ep);
        _router->addPort(addr, port);
        
        _router->addRouting(addr, 1, addr);
    });
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

