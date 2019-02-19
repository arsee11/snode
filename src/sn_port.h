///sn_port.h

#ifndef SN_PORT_H
#define SN_PORT_H

#include <memory>
#include <functional>
#include "sn_udptransport.h"

namespace snode
{
class Message;
class Port;
using port_ptr = std::shared_ptr<Port>;
using OnInputCb = std::function<void (Port* port, const Message& msg)>;


///@brief Interface of the router. \n
///input message from outside to router for forwarding, \n
///and output message from router to outside.
class Port
{
public:
	virtual int output(const Message& m)=0;

	virtual int input(const Message& m)=0;

    void setInputCallBack(const OnInputCb& cb){ _input_cb = cb; }

protected:
    OnInputCb _input_cb=nullptr;
};



////////////////////////////////////////////////////////////
///TCP port
class TcpPort:public Port
{
};



////////////////////////////////////////////////////////////
///UDP port
class UdpPort:public Port
{
public:
    UdpPort(UdpTransport* transport, const TransEndpoint& local, const TransEndpoint& remote);
	
	int output(const Message& m)override;

    int input(const Message& m)override;

private:
	int output(const void* data, size_t len);
    void onTransportRecv(const NetMsg &msg);


private:
    UdpPeer* _peer;
    TransEndpoint _remote;
};



////////////////////////////////////////////////////////////
///Local port
class LocalPort:public Port
{
};

}

#endif /*SN_PORT_H*/
