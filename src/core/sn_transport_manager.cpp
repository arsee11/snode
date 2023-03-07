///sn_transport_manager.cpp
///

#include "sn_transport_manager.h"

namespace snode{

TransportManager::TransportManager()
{
   _thread.name("transport"); 
   _thread.post(std::bind(&TransportManager::evtLoop, this));
}

void TransportManager::clear()
{
    _is_running = false;
    _thread.stop();
}

void  TransportManager::evtLoop()
{
    _evt_queue.process();
    if(_is_running){
        _thread.post(std::bind(&TransportManager::evtLoop, this));
    }
}

UdpTransport* TransportManager::getUdpTransport(const TransEndpoint& local)
{
    UdpPeer * udp = new UdpPeer(&_evt_queue, local);
    udp->open();

    return new UdpTransport(std::shared_ptr<UdpPeer>(udp), local);
}

UdpTransport* TransportManager::getUdpTransport(const std::string& lip)
{
    UdpPeer * udp = new UdpPeer(&_evt_queue, TransEndpoint{lip, 0});
    udp->open();

    return new UdpTransport(std::shared_ptr<UdpPeer>(udp), udp->local_addr());
}

TransportServer *TransportManager::getUdpTransportServer(const TransEndpoint &local)
{
    UdpPeer * udp = new UdpPeer(&_evt_queue, local);
    udp->open();

    auto t = new UdpTransportServer(std::shared_ptr<UdpPeer>(udp), local);
    t->open();
    return t;
}

}
