///sn_router.h

#ifndef SN_ROUTER_H
#define SN_ROUTER_H

#include "sn_port.h"
#include <memory>
#include <assert.h>
#include <list>
#include <algorithm>

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
		//port_ptr port = _routing_method.port();
		//addport(port);
		//addRouting(local(), local(), 1, port);
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
    ///@param mectric mectric of this route path
    ///@param port the router's port for input/output
    void  addRouting(const Address& dst, int metric, const port_ptr& port)
	{

        auto i = std::find(_ports.begin(), _ports.end(), port);
        if(i != _ports.end() )
            _route_table.add(dst, metric, port);
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

private:
    void onPortInput(Port* srcport, const Message& msg){

		if( forward(msg) <=0 )
		{
			//srcport->output(NotReachableMsg());
		}
	}

	RoutingMethod  _routing_method;
	RouteTable _route_table;
    std::list<port_ptr> _ports;
};

}

#endif /*SN_ROUTER_H*/
