#ifndef SN_TRANSPORT_H
#define SN_TRANSPORT_H

#include <easynet/udppeer.h>
#include <ostream>

namespace snode {

using TransEndpoint = arsee::net::AddrPair;
using UdpPeer = arsee::net::UdpPeer4;
using NetMsg = arsee::net::MsgSt;
using NetEventQueue = arsee::net::EventQueueEpoll;
using IP = arsee::net::IP;

inline std::ostream& operator<<(std::ostream& os, const TransEndpoint& ep){
	os<<"{"<<ep.ip<<":"<<ep.port<<"}";
	return os;
}

//////////////////////////////////////////////////////////////////////////
class Transport
{
    using RecvCb = std::function<void(const void* data, int size)>;
public:
    virtual ~Transport()=default;

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

using trans_ptr = std::shared_ptr<Transport>;

//////////////////////////////////////////////////////////////////////////
class TransportServer
{
    using OnConnectedCb = std::function<void(trans_ptr)>;
public:
    virtual ~TransportServer()=default;
    virtual TransEndpoint local_ep()const=0;
    virtual bool open()=0;
    void listenOnConnect(OnConnectedCb cb){ _conn_cb = cb; }

protected:
    OnConnectedCb _conn_cb;
};


//////////////////////////////////////////////////////////////////////////
class NetTransport : public Transport
{
public:
    NetTransport()=default;
    NetTransport(const TransEndpoint& local)
        :_local_ep(local)
    {
        if(IP::is_any_ip(local.ip)){
            _local_ep.ip = IP::get_a_host_ipv4();
        }
    }

    TransEndpoint local_ep()const override{ 
        return _local_ep; 
    }
    TransEndpoint remote_ep()const override{ return _remote_ep; }
    void remote_ep(const TransEndpoint& ep){ _remote_ep = ep; }

protected:
    TransEndpoint _local_ep;
    TransEndpoint _remote_ep;
};

}//snode

#endif // SN_TRANSPORT_H
