//sn_snode_impl.h

#ifndef SN_SNODE_IMPL_H
#define SN_SNODE_IMPL_H

#include "sn_snode.h"
#include "core/sn_address_manager.h"
#include "core/sn_transport_manager.h"
#include "sn_router_impl.h"

namespace snode {

class CommandSession;

class SnodeImpl : public Snode
{
public:
    SnodeImpl(RouterImpl* router,
              AddressManager* addressmgr,
              TransportManager* transportmgr);
    virtual ~SnodeImpl(){}
    void setAddress(const Address& addr)override;
    bool setupCommandTransport(const std::string& local_ip, int local_port)override;
    void setNeighbor(const Neighbor& neib)override;
    void addStaticRoute(const Address& dst, int metric, const Address& next_hop)override;
    Address directlyRegister(Transport* trans)override;

protected:
    RouterImpl* _router=nullptr;
    AddressManager* _addressmgr;
    TransportManager* _transportmgr;
    Address _address;
    std::string _local_forward_ip;
    std::string _local_cmd_ip;
    int _local_cmd_port=0;
};


template<class RoutingMethod>
class SnodeImplT : public SnodeImpl
{
private:
    RoutingMethod _routing_method;
};

}//namespace snode


#endif /*SN_SONDE_IMPL_H*/
