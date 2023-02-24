#ifndef SN_TRANSPORT_H
#define SN_TRANSPORT_H

#include <easynet/udppeer.h>

namespace snode {

using TransEndpoint = arsee::net::AddrPair;
using UdpPeer = arsee::net::UdpPeer4;
using NetMsg = arsee::net::MsgSt;
using NetEventQueue = arsee::net::EventQueueEpoll;

//////////////////////////////////////////////////////////////////////////
class Transport
{
    using RecvCb = std::function<void(const void* data, int size)>;
public:
    //virtual bool open()=0;
    //virtual bool close()=0;
    virtual TransEndpoint local_ep()const=0;
    virtual TransEndpoint remote_ep()const=0;
    virtual void remote_ep(const TransEndpoint& ep)=0;
    virtual int send(const void* data, int size)=0;
    void listenOnRecv(RecvCb cb){ _recv_cb = cb; }

protected:
    RecvCb _recv_cb;
};




//////////////////////////////////////////////////////////////////////////
class NetTransport : public Transport
{
public:
    NetTransport(const TransEndpoint& local)
        :_local_ep(local)
    {
    }

    TransEndpoint local_ep()const override{ return _local_ep; }
    TransEndpoint remote_ep()const override{ return _remote_ep; }
    void remote_ep(const TransEndpoint& ep){ _remote_ep = ep; }

protected:
    TransEndpoint _local_ep;
    TransEndpoint _remote_ep;
};

}//snode

#endif // SN_TRANSPORT_H
