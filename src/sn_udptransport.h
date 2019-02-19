//sn_udptransport.h


#ifndef SN_UDP_H
#define SN_UDP_H

#include <functional>
#include <map>
#include <unordered_map>

#include "sn_transport.h"

namespace snode
{

class UdpTransport
{
    using OnRecvCb = std::function<void (const NetMsg& msg) >;
public:
    UdpTransport(NetEventQueue* eq)
        :_evt_queue(eq)
        ,_listener(eq)
    {}

    UdpPeer* registerPeer(const TransEndpoint &local, const TransEndpoint& peer, OnRecvCb onrecv);
    void unregisterPeer(UdpPeer *udp);
    void close(){}

private:
    void onRecv(UdpPeer* udp, const TransEndpoint& remote, NetMsg msg);
    UdpPeer *findPeer(const TransEndpoint &local);
    void addPeer(const TransEndpoint &local, UdpPeer *peer);
    void delPeer(UdpPeer *peer);

private:
    using EpRecvCbMap = std::unordered_map<TransEndpoint, OnRecvCb, arsee::net::addr_hash_value>;
    using PeerRecvCbMap = std::map<UdpPeer*, EpRecvCbMap>;
    PeerRecvCbMap _peer_recvcb_map;
    NetEventQueue* _evt_queue;
    UdpInpuListner _listener;

    ///@key local TransEndpoint
    std::unordered_map<TransEndpoint, UdpPeer*, arsee::net::addr_hash_value> _peer_map;

};

}//namespace snode

#endif /*SN_UDP_H*/
