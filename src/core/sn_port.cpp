///sn_pport.cpp

#include "sn_port.h"
#include "sn_transport.h"
#include "sn_message.h"
#include <iostream>

namespace snode
{

void Port::setTransport(Transport* t)
{
    _transport = t;
    if(_transport != nullptr){
        _transport->listenOnRecv(
            [this](const void* data, int size)
            {
                Message msg;
                msg.decode((const uint8_t*)data, size);
                this->input(msg);
            }
        );

    }
}

int Port::output(const void* data, size_t len)
{
	if(_transport == nullptr)
	{
        //log_error()<<"output failed, transport not initialized"<<log_end();
        std::cout<<"output failed, transport not initialized\n";
		return -1;
	}

    _transport->send(data, len);

    return len;
}



int Port::output(const Message& m)
{
    uint8_t* stream=nullptr;
    size_t size=0;
    std::tie(stream, size) = m.encode();
    return output( stream, size );
}

int Port::input(const Message &m)
{
    std::cout<<"port local ep"<<_transport->local_ep()<<"size="<<m.size()<<std::endl; 
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


}//namespace snode
