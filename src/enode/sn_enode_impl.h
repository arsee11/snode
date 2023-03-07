///sn_enode_impl.h

#ifndef SN_EONDE_IMPL_H
#define SN_EONDE_IMPL_H

#include "sn_enode.h"
#include "core/sn_transport_manager.h"
#include "sn_command_session.h"

namespace snode {

class ENodeImpl : public ENode
{
    using RecvCb = std::function<void(const Message& msg)>;
    using ConnectCb = std::function<void(int ec, const char* msg)>;

public:
    ENodeImpl(const std::string& lip);
    void listenOnRecv(RecvCb cb)override;
    void connect(const TransEndpoint& snode_ep, ConnectCb cb)override;
    int sendTo(const void* data, int len, const Address& peer)override;
    Address address()const override{ return _addr; }
    TransEndpoint configAddress(const Address &addr, const TransEndpoint &remote_ep)override;

private:
    RecvCb _recv_cb;
    ConnectCb _conn_cb;
    Address _addr;
    std::string _local_ip;
    std::unique_ptr<NetTransport> _data_transport;
    std::unique_ptr<enode::CommandSession> _cmd_session;
    std::unique_ptr<TransportManager> _transportmgr;
};

}//namespace snode

#endif //SN_EONDE_IMPL_H
