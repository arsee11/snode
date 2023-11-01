///sn_neighbor.h

#ifndef SN_NEIGHBOR_H
#define SN_NEIGHBOR_H

#include "core/sn_address.h"
#include "core/sn_transport.h"
#include "core/sn_port.h"
#include "sn_snode.h"
#include "core/sn_timer.h"

namespace snode {

struct Neighbor
{
    std::string name;
    Address addr;  //remote (the Neighbor)'s Address
    port_ptr port; //the local port which is used to send/recv message to neighbor                
    TransEndpoint comminucate_ep;   //remote(the Neighbor)'s endpoint for say hello
};

inline bool operator==(const Neighbor& lhs, const Neighbor& rhs){
    return ( lhs.addr == rhs.addr );
}

using NeighborList=std::list<Neighbor>;

class CommandSession;

class NeighborManager
{
protected:
    struct NeighborWapper
    {
        Neighbor neib;
        std::shared_ptr<AwaitTimer> keepalive_timer;
        std::shared_ptr<AwaitTimer> hello_timer;
        bool disable=false;
   
        ~NeighborWapper(){
            disable=true;
            keepalive_timer->stop();
            hello_timer->stop();
        }
    };
    using neighbor_ptr=std::shared_ptr<NeighborWapper>;
    using NeighborMap=std::map<Address, neighbor_ptr>;

public:
    NeighborManager(Snode* sn)
        :_sn(sn)
    {
        
    }

    virtual ~NeighborManager()=default;

    void initNeighbors(CommandSession* ss, const NeighborList& ns);
    void onNeighborUpdate(CommandSession* ss, const Address& neib_addr, const TransEndpoint& msg_ep);

private:
    virtual void addNeighbor(const Neighbor& neib)=0;
    void disableNeigbor(neighbor_ptr neib);
    AwaitTimer::co_task keepalive(neighbor_ptr neib);
    AwaitTimer::co_task helloToNeighbor(CommandSession* ss, neighbor_ptr neib);


protected:
    Snode* _sn=nullptr;
    NeighborMap _neighbors;
};


template<class ThreadingScope>
class NeighborManagerT: public NeighborManager
{
public:
    NeighborManagerT(Snode* sn, ThreadingScope* thr_scope)
        :NeighborManager(sn)
        ,_thr_scope(thr_scope)
    {
        
    }

private:
    void addNeighbor(const Neighbor &neib){
        port_ptr port = _sn->newPort();
        std::shared_ptr<AwaitTimer> ktimer(new AwaitTimer(_thr_scope->poller()));
        std::shared_ptr<AwaitTimer> htimer(new AwaitTimer(_thr_scope->poller()));
        neighbor_ptr nptr(new NeighborWapper{neib, ktimer, htimer});
        nptr->neib.port = port;
        _neighbors[neib.addr] = nptr;
    }  

private:
    ThreadingScope* _thr_scope;
};

}//namespace snode

#endif/*SN_NEIGHBOR_H*/
