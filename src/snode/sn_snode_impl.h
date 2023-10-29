//sn_snode_impl.h

#ifndef SN_SNODE_IMPL_H
#define SN_SNODE_IMPL_H

#include "sn_snode.h"
#include "core/sn_address_manager.h"
#include "core/sn_transport_manager.h"
#include "sn_router_impl.h"

namespace snode {

class CommandSession;

//thread safty
class SnodeImpl : public Snode
{
public:
    SnodeImpl(RouterImpl* router,
              AddressManager* addressmgr,
              TransportManager* transportmgr);
    virtual ~SnodeImpl();
    Address getAddress()const override;
    bool setupCommandTransport(const std::string& local_ip, int local_port)override;
    void addNeighbor(const Neighbor& neib)override;
    void configNeighbor(const Address& neib_addr, const TransEndpoint& neib_port_ep)override;
    NeighborMap getNeighbors()const override;
    void addEnode(Address addr, port_ptr port, const TransEndpoint& remote_ep)override;
    void addStaticRoute(const Address& dst, int metric, const Address& next_hop)override;
    port_ptr newPort()override;
    Address allocAddress()override;
    void releaseAddress(const Address& addr)override;
    
protected:
    RouterImpl* _router=nullptr;
    AddressManager* _addressmgr;
    TransportManager* _transportmgr;
    Address _address;
    std::string _local_forward_ip;
    std::string _local_cmd_ip;
    int _local_cmd_port=0;
    std::map<Address, Neighbor> _neighbors;


    EventQueueEpoll _event_eq;
    std::unique_ptr<ThreadScopePolling> _thrscope;
};


template<class RoutingMethod>
class SnodeImplT : public SnodeImpl
{
private:
    RoutingMethod _routing_method;
};

}//namespace snode


#endif /*SN_SONDE_IMPL_H*/
