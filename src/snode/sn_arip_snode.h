//sn_snode.h

#ifndef SN_SONDE_H
#define SN_SONDE_H

namespace snode {

class Snode
{
public:
    virtual ~Snode(){};
    virtual void setAddress(const Address& addr)=0;
    virtual bool setupCommandTransport(const std::string& local_ip, int local_port)=0;
    virtual void setNeighbours(const NeighbourMap& ns)=0;
    virtual void addStaticRoute(const Address& dst, int metric, const Address& next_hop)=0;
    virtual void addStaticRoute(const Address& dst, int metric, const Address& next_hop)=0;
    virtual Address directlyRegister(Transport* trans)=0;
};


extern Snode* createSnode();

}//namespace snode

#endif /*SN_SONDE_H*/
