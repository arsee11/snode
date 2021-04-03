///sn_transport_manager.cpp
///

#include "sn_transport_manager.h"

namespace snode{

TransportManager::TransportManager()
{
   _thread.name("transport"); 
   _thread.post(std::bind(&TransportManager::evtLoop, this));
}

void  TransportManager::evtLoop()
{
    _evt_queue.process();
   _thread.post(std::bind(&TransportManager::evtLoop, this));
}

UdpTransport* TransportManager::getUdpTransport(const TransEndpoint& local)
{
    UdpPeer * udp = new UdpPeer(&_evt_queue, local);
    udp->open();

    return new UdpTransport(udp, local);
}


}
