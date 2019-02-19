///sn_transport_manager.cpp
///

#include "sn_transport_manager.h"

namespace snode{

TransportManager::TransportManager()
{
    _udpt = new UdpTransport(&_evt_queue);
}

void TransportManager::run()
{
    _evt_queue.process();
}


}
