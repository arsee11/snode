//sn_udptransport.h


#ifndef SN_UDP_TRANSPORT_H
#define SN_UDP_TRANSPORT_H

#include "sn_transport.h"
#include <unordered_map>

namespace snode
{
///////////////////////////////////////////////////////////////////////
/// \brief The UdpTransport class
///
class UdpTransport : public NetTransport
{
public:
    UdpTransport()=default;
    UdpTransport(const std::shared_ptr<UdpPeer>& udp, const TransEndpoint &local)
        :NetTransport(local)
        ,_udp(udp)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        _udp->listenOnRecv(std::bind(&UdpTransport::onRecv, this, _1, _2));
    }

    ~UdpTransport(){
        _udp->close();
    }
    int send(const void* data, int size)override;

protected:
    void onRecv(const TransEndpoint& remote, const NetMsg& msg);

protected:
    std::shared_ptr<UdpPeer> _udp;
};


///////////////////////////////////////////////////////////////////////
/// \brief The UdpTransportServer class
///
class UdpTransportServer : public TransportServer
{
    class UdpTransportClient : public UdpTransport
    {
    public:
        UdpTransportClient(const std::shared_ptr<UdpPeer>& udp, const TransEndpoint& local);
        void onRecv(const NetMsg& msg);
    };

    using udpcli_ptr = std::shared_ptr<UdpTransportClient>;

public:
    UdpTransportServer(const std::shared_ptr<UdpPeer>& udp, const TransEndpoint &local)
        :_local_ep(local)
        ,_udp(udp)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        _udp->listenOnRecv(std::bind(&UdpTransportServer::onRecv, this, _1, _2));
    }
    bool open()override;
    TransEndpoint local_ep()const override{ return _local_ep; }

private:
    void onRecv(const TransEndpoint& remote, const NetMsg& msg);
    TransEndpoint _local_ep;
    std::shared_ptr<UdpPeer> _udp;

    std::unordered_map<TransEndpoint, udpcli_ptr, arsee::net::addr_hash_value> _clients;
};

}//namespace snode

#endif /*SN_UDP_TRANSPORT_H*/
