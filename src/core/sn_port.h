///sn_port.h

#ifndef SN_PORT_H
#define SN_PORT_H

#include <memory>
#include <functional>
#include "sn_transport.h"
#include "sn_address.h"

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
    //take the onwership of transport 
    void setTransport(Transport* t);
    Transport* transport(){ return _transport.get(); }
	int output(const Message& m);

	int input(const Message& m);

    void setInputCallBack(const OnInputCb& cb){ _input_cb = cb; }

private:
    int output(const void* data, size_t len);

protected:
    //Address _addr;
    std::unique_ptr<Transport> _transport;
    OnInputCb _input_cb=nullptr;
};

using port_ptr = std::shared_ptr<Port>;

}//snode

#endif /*SN_PORT_H*/
