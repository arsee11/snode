//sn_snode.h

#ifndef SN_SONDE_H
#define SN_SONDE_H

#include "core/sn_address.h"
#include "core/sn_transport.h"
#include "core/sn_port.h"
#include "sn_neighbor.h"

namespace snode {

class Snode
{
public:
    virtual ~Snode()=default;
    virtual Address getAddress()const=0;
    virtual bool setupCommandTransport(const std::string& local_ip, int local_port)=0;
    virtual void addNeighbor(const Neighbor& neib)=0;
    virtual void configNeighbor(const Address& neib_addr, const TransEndpoint& neib_port_ep)=0;
    virtual NeighborMap getNeighbors()const=0;
    virtual void addEnode(Address addr, port_ptr port, const TransEndpoint& remote_ep)=0;
    virtual void addStaticRoute(const Address& dst, int metric, const Address& next_hop)=0;
    virtual port_ptr newPort()=0;
    virtual Address allocAddress()=0;
    virtual void releaseAddress(const Address& addr)=0;
};

}//namespace snode

#endif /*SN_SONDE_H*/
