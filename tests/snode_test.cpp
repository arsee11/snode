///snode_test.cpp

#include "core/sn_transport_manager.h"
#include "core/sn_address_manager.h"
#include "core/sn_port.h"
#include "core/sn_message.h"
#include "core/sn_router.h"
#include "core/sn_route_table.h"
#include "core/sn_rip_routing.h"
#include "snode/sn_json_serializer.h"
#include "commands.h"

#include <boost/json.hpp>

using namespace snode;

#include <iostream>
using namespace std;
using namespace std::placeholders;
using RouterImpl=Router<RIPRouting, RouteTable>;

RouterImpl router;
TransportManager transportmgr;
AddressManager addressmgr(Address(1u, 0u));
CommandParser parser;

std::string local_ip="127.0.0.1";
UdpPeer* service_trans=nullptr;

struct Neighbour
{
    Address addr;
    TransEndpoint endpoint;
};

using NeighbourMap=std::list<Neighbour>; 
NeighbourMap neighbours{
    {Address(2u, 1u), {"192.168.127.185", 10011}}
	
};
    

struct Transaction
{
    Transaction(const std::string& id_in):id(id_in){}
    virtual ~Transaction(){};

    virtual void commit()=0;
    virtual void rollback()=0;

    std::string id;
};

using transaction_ptr=std::shared_ptr<Transaction>;
static std::map<std::string, transaction_ptr> transaction_map;
void addTransaction(transaction_ptr t){
    transaction_map[t->id] = t;
}

transaction_ptr getTransaction(const std::string& id){
    return transaction_map[id];
}


class RegisterTransaction : public Transaction
{
public:
    RegisterTransaction(const std::string& id)
        :Transaction(id)
    {
    }

    void addressSent(port_ptr port, const Address& addr){
        _alloced_addr = addr;
        _port = port;
    }

    void setRemoteEndpoint(const TransEndpoint& ep){
        _remote_ep = ep;
    }

    void commit()override{
        static_cast<NetTransport*>(_port->transport())->remote_ep(_remote_ep);
	    router.addPort(_port);
	    router.addRouting(_alloced_addr, 1, _alloced_addr, _port);
    }

    void rollback()override{
        addressmgr.releaseAddress(_alloced_addr);
    }

private:
    Address _alloced_addr; 
    TransEndpoint _remote_ep; 
    port_ptr _port;
};

std::shared_ptr<RegisterTransaction> rtransaction;
cmd_ptr onRegister(Command* req)
{
    rtransaction.reset(new RegisterTransaction(req->id) );
	Transport* udp = transportmgr.getUdpTransport(local_ip);

    Address addr = addressmgr.allocAddress();
	port_ptr port = std::make_shared<Port>();
    //port->setAddress(addr);
    port->setTransport(udp);

    AddressConfigCmd* cmd = new AddressConfigCmd;
    cmd->id = req->id;
    cmd->address = addr.raw();
    cmd->ip = local_ip;
    cmd->port = udp->local_ep().port;
    rtransaction->addressSent(port, addr);
    return cmd_ptr(cmd);
}


cmd_ptr onAddressConfirm(Command* req)
{
    auto cmd = static_cast<AddressConfirmCmd*>(req);
    TransEndpoint endpoint{cmd->ip, cmd->port};
    rtransaction->setRemoteEndpoint(endpoint);
    rtransaction->commit();

    OKCmd* rsp = new OKCmd;
    rsp->id = req->id;
    return cmd_ptr(rsp);
}

cmd_ptr onCmdOK(Command* req)
{
    return nullptr;
}


void onRecvCmd(const TransEndpoint& remote_ep, const NetMsg& msg)
{
    std::cout<<"Recv cmd:";
    std::cout<<(const char*)msg.rd_ptr()<<endl;

    cmd_ptr req = std::move(parser.parse((const char*)msg.rd_ptr(), msg.size())); 
    cmd_ptr rsp = std::move(req->dispatch());

    if(rsp != nullptr){
        CommandEncoder* cmde = rsp->encoder();
        service_trans->sendTo(remote_ep, NetMsg(cmde->buf(), cmde->size()));
        std::cout<<"send rsp:"<<(char*)cmde->buf()<<endl;
    }
}

void init()
{
    parser.setDispatcher(
        CmdDispatcher<RegisterCmd>{std::bind(&onRegister, _1)}
    );    
    parser.setDispatcher(
        CmdDispatcher<AddressConfirmCmd>{std::bind(&onAddressConfirm, _1)}
    );    
    parser.setDispatcher(
        CmdDispatcher<OKCmd>{std::bind(&onCmdOK, _1)}
    );    
}

void updateRoute()
{
}

void dumpRoute()
{
    cout<<"------------routes-----------------\n";
    JsonRouteSerializer<RouterImpl> s(&router);
    cout<<s()<<endl;
}

void setNeighbours(const NeighbourMap& ns)
{
    for(auto& neib : ns){
	    UdpTransport* udp = transportmgr.getUdpTransport(TransEndpoint{local_ip, 10010});
        udp->remote_ep(neib.endpoint);

	    port_ptr port = std::make_shared<Port>();
        port->setTransport(udp);
	    router.addPort(port);

	    router.addRouting(neib.addr, 1, neib.addr, port);
    }
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"usage: [local ip] [local port]"<<endl;
		return 0;
	}

	local_ip = argv[1];
	uint16_t port= atoi(argv[2]);
    TransEndpoint service_ep{local_ip, port};

    init();
    setNeighbours(neighbours);

    NetEventQueue eq;
    service_trans = new UdpPeer(&eq, service_ep);
    service_trans->open();
    service_trans->listenOnRecv(&onRecvCmd);

    bool running=true;
    std::thread t([&running, &eq]() {
	    while(running){
		    eq.process();
	    }
    });

    char ch;
    while(ch != 'q'){
        cout<<">>";
        cin >> ch;
        if(ch == 'd'){
            dumpRoute();
        }
    }

    running = false;
    t.join();
    transportmgr.clear();

	return 0;
}
