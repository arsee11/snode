///sn_neighbor.h

#ifndef SN_NEIGHBOR_H
#define SN_NEIGHBOR_H

#include <fstream>
#include <boost/json.hpp>
#include "core/sn_address.h"
#include "core/sn_transport.h"
#include "core/sn_routing.h"

namespace snode {

using NeighborMap=std::list<Neighbor>;

void readNeighbors(NeighborMap& ns);


}//namespace snode

#endif/*SN_NEIGHBOR_H*/
