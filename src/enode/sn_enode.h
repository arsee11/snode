///sn_enode.h

#ifndef SN_EONDE_H
#define SN_EONDE_H

#include <iostream>
#include "core/sn_transport.h"
#include "core/sn_address.h"
#include "core/sn_message.h"

namespace snode {

class ENode
{
    using RecvCb = std::function<void(const Message& msg)>;
    using ConnectCb = std::function<void(int ec, const char* msg)>;

public:
    ENode()=default;
    virtual ~ENode()=default;
    virtual void listenOnRecv(RecvCb cb)=0;
    virtual void connect(const TransEndpoint& snode_ep, ConnectCb cb)=0;
    virtual void sendTo(const void* data, int len, const Address& peer)=0;
    virtual Address address()const=0;
    virtual TransEndpoint configAddress(const Address& addr, const TransEndpoint &remote_ep)=0;
};

}//namespace snode

#endif //SN_EONDE_H
