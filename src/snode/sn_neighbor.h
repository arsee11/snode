///sn_neighbor.h

#ifndef SN_NEIGHBOR_H
#define SN_NEIGHBOR_H

#include <fstream>
#include <boost/json.hpp>
#include "core/sn_address.h"
#include "core/sn_transport.h"
#include "core/sn_port.h"

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

using NeighborMap=std::list<Neighbor>;

void readNeighbors(NeighborMap& ns);


}//namespace snode

#endif/*SN_NEIGHBOR_H*/
