///sn_router.h

#ifndef SN_ROUTER_H
#define SN_ROUTER_H

#include "sn_port.h"
#include <memory>
#include <assert.h>
#include <list>
#include <algorithm>
#include <iostream>

namespace snode
{
class Address;

using port_ptr = std::shared_ptr<Port>;

template<class RoutingMethod, class RouteTable>
class Router
{
public:
	Router()
        //:_routing_method(&_route_table, this)
	{
		//setup local port for RoutingMethod,
		//receive route exchange msg from other routers.
	}

	int forward(const Message& msg){ 
		
        port_ptr port = _route_table.routing(msg.dst());
		if(port != nullptr)
			return port->output(msg);
		
		//log 
		return 0;
	}

	bool start();

    ///@brief add static route item
    ///@param dst dst snode/node address
    ///@param metric mectric of this route path
    ///@param next_hop next snode for forwarding
    ///@param port the router's port for input/output
    void addRouting(const Address& dst, int metric,
                     const Address& next_hop, const port_ptr& port
    )
	{

        auto i = std::find(_ports.begin(), _ports.end(), port);
        if(i != _ports.end() )
            _route_table.add(dst, metric, next_hop, port);
		else
            throw std::runtime_error("port not found!");
	}

	void addPort(const port_ptr& port){

		assert(port.get());
        port->setInputCallBack([this](Port* srcport, const Message& msg)
			{ 
				this->onPortInput(srcport, msg); 
			});
	
		_ports.push_back(port);
	}

    const RouteTable& route_table()const{ return _route_table; }

private:
    void onPortInput(Port* srcport, const Message& msg){

		if( forward(msg) <=0 )
		{
			//srcport->output(NotReachableMsg());
            std::cout<<"No route for dst["<<std::hex<<msg.dst().raw()
                     <<"] from src["<<msg.src().raw()<<std::dec<<"]\n";
		}
	}

	RoutingMethod  _routing_method;
	RouteTable _route_table;
    std::list<port_ptr> _ports;
};

}

#endif /*SN_ROUTER_H*/
