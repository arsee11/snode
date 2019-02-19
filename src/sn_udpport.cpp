///sn_udpport.cpp

#include "sn_port.h"
#include "sn_udptransport.h"
#include "sn_message.h"

namespace snode
{

UdpPort::UdpPort(UdpTransport *transport, const TransEndpoint &local, const TransEndpoint &remote)
    :_remote(remote)
{
    using namespace std::placeholders;
    _peer = transport->registerPeer(local, remote, std::bind(&UdpPort::onTransportRecv, this, _1));
}
	
int UdpPort::output(const void* data, size_t len)
{
	if(_peer == nullptr)
	{
        //log_error()<<"UpdPort::output failed, not initialized"<<log_end();
		return -1;
	}

    NetMsg msg(data, len);
    _peer->sendTo(_remote, msg);

    return len;
}



int UdpPort::output(const Message& m)
{
    uint8_t* stream=nullptr;
    size_t size=0;
    std::tie(stream, size) = m.encode();
    return output( stream, size );
}

int UdpPort::input(const Message &m)
{
    if(this->_input_cb != nullptr)
    {
        this->_input_cb(this, m);
        return m.size();
    }
    else
    {
        //log_warn()<<"UpdPort::input call back is nullptr"<<log_end();
        return 0;
    }
}


void UdpPort::onTransportRecv(const NetMsg& nmsg)
{
    Message msg;
    msg.decode(nmsg.begin(), nmsg.size());
    this->input(msg);
}

}//namespace snode
