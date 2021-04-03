//sn_udptransport.h


#ifndef SN_UDP_TRANSPORT_H
#define SN_UDP_TRANSPORT_H

#include "sn_transport.h"

namespace snode
{

class UdpTransport : public NetTransport
{
public:
    UdpTransport(UdpPeer* udp, const TransEndpoint &local)
        :NetTransport(local)
        ,_udp(udp)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        _udp->listenOnRecv(std::bind(&UdpTransport::onRecv, this, _1, _2));
    }

    int send(const void* data, int size)override{
        if(_udp != nullptr){
            _udp->sendTo(_remote_ep, NetMsg(data, size));
            return size;
        }

        return 0;
    }

private:
    void onRecv(const TransEndpoint& remote, const NetMsg& msg){
        if(remote == _remote_ep){
            if(_recv_cb != nullptr){
                _recv_cb(msg.rd_ptr(), msg.size());
            }
        }
    }

private:
    UdpPeer* _udp=nullptr;
};

}//namespace snode

#endif /*SN_UDP_TRANSPORT_H*/
