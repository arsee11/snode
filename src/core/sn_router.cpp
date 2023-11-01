﻿///sn_router.cpp

#include "sn_router.h"

#include "sn_port.h"
#include "sn_message.h"
#include <memory>
#include <assert.h>
#include <algorithm>
#include <iostream>

namespace snode
{

void Router:: addPort(const Address &next_hop, const port_ptr& port)
{
	assert(_thrscope != nullptr);
	assert(_thrscope->isInMyScope());

    assert(port.get());
	port->setInputCallBack([this](Port* srcport, const Message& msg)
	{ 
		_thrscope->post(&Router::onPortInput, this, srcport, msg); 
	});

    _ports[next_hop] = port;
}


port_ptr Router::findPort(const Address &next_hop)
{
	assert(_thrscope != nullptr);
	assert(_thrscope->isInMyScope());

    return _ports[next_hop];
}

void Router::delPort(const Address &next_hop)
{
	assert(_thrscope != nullptr);
	assert(_thrscope->isInMyScope());

	_ports[next_hop] = nullptr;
}

void Router::onPortInput(Port* srcport, const Message& msg)
{
	assert(_thrscope != nullptr);
	assert(_thrscope->isInMyScope());

	if( Message::is_link_update_message(msg)){
		onLinkUpdate(msg);
	}
	else{
		if( forward(msg) <=0 )
		{
			//srcport->output(NotReachableMsg());
			std::cout<<"No route for dst["<<std::hex<<msg.dst().raw()
					<<"] from src["<<msg.src().raw()<<std::dec<<"]\n";
		}
	}
}

}

