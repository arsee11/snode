///sn_neighbor.h

#ifndef SN_NEIGHBOR_H
#define SN_NEIGHBOR_H

#include <fstream>
#include <boost/json.hpp>
#include "core/sn_address.h"
#include "core/sn_transport.h"

namespace snode {

struct Neighbor
{
    Address addr;           //remote (the Neighbor)'s Address
    uint16_t lport;         //local endpoint port
    TransEndpoint rendpoint;//remote(the Neighbor)'s endpoint
};

using NeighborMap=std::list<Neighbor>;

void readNeighbors(NeighborMap& ns);

}//namespace snode

#endif/*SN_NEIGHBOR_H*/
