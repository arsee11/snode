//sn_udptransport.cpp

#include "sn_udptransport.h"

namespace snode
{

UdpPeer *UdpTransport::registerPeer(const TransEndpoint &local, const TransEndpoint &remote, UdpTransport::OnRecvCb onrecv)
{
    UdpPeer* udp = findPeer(local);
	if( udp == nullptr )
	{
		try{
            using namespace std::placeholders;
            udp = new UdpPeer(_evt_queue, local);
            udp->open();
            _listener.listen(udp->fd(), std::bind(&UdpTransport::onRecv, this, udp, _1, _2));
        }catch(arsee::net::sockexcpt& e){
            //log_error()<<"UdpManager::register "<<e.what()<<end_log();
			return nullptr;
		}
	}

    addPeer(local, udp);
    const auto& it = _peer_recvcb_map.find(udp);
    if(it == _peer_recvcb_map.end())
    {
        EpRecvCbMap epmap;
        epmap.insert( {remote, onrecv});
        _peer_recvcb_map[udp] = epmap;
    }
    else
        it->second.insert({remote, onrecv});

    return udp;
}

void UdpTransport::unregisterPeer(UdpPeer *udp)
{
    _peer_recvcb_map.erase(udp);
    delPeer(udp);
    delete udp;
}

void UdpTransport::onRecv(UdpPeer* udp, const TransEndpoint &remote, NetMsg msg)
{
    const auto& it = _peer_recvcb_map.find(udp);
    if(it != _peer_recvcb_map.end())
    {
        auto& ep_recvcb_map = it->second;
        auto irecvcb = ep_recvcb_map.find(remote);
        if(irecvcb != ep_recvcb_map.end())
            irecvcb->second(msg);
    }

}

UdpPeer* UdpTransport::findPeer(const TransEndpoint& local)
{
    const auto& it = _peer_map.find(local);
    if(it != _peer_map.end())
        return it->second;

    return nullptr;
}

void UdpTransport::addPeer(const TransEndpoint& local, UdpPeer* peer)
{
    _peer_map.insert( {local, peer});
}

void UdpTransport::delPeer(UdpPeer* peer)
{
    for(auto i=_peer_map.begin(); i!=_peer_map.end(); ++i)
    {
        if(i->second == peer)
        {
            _peer_map.erase(i);
            break;
        }
    }
}

}//namespace snode
