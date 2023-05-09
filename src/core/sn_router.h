///sn_router.h

#ifndef SN_ROUTER_H
#define SN_ROUTER_H

#include "sn_port.h"
#include "sn_message.h"
#include <map>

namespace snode
{
class Address;

using port_ptr = std::shared_ptr<Port>;

/// @brief 
class Router
{
public:
    Router() =default;

    virtual void addDirectLink(const Address& next_hop, const port_ptr& port)=0;

protected:
    void addPort(const Address& next_hop, const port_ptr& port);
    port_ptr findPort(const Address& next_hop);
    void onPortInput(Port* srcport, const Message& msg);
    virtual void onLinkUpdate(const Message& msg)=0;

    ///Binding the next hop addresses and ports
    std::map<Address, port_ptr> _ports;
};


/// @brief 
/// @tparam RouteTable 
template<class RouteTable>
class RouterT : public Router
{
public:
	RouterT()=default;
	int forward(const Message& msg){ 
			
		port_ptr port = _route_table.routing(msg.dst());
		if(port != nullptr)
			return port->output(msg);
		
		//log 
		return 0;
	}

    const RouteTable& route_table()const{ return _route_table; }

protected:
	RouteTable _route_table;
};


}

#endif /*SN_ROUTER_H*/
