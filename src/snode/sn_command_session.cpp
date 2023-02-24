//sn_command_session.cpp

#include "sn_command_session.h"
#include "core/sn_address_manager.h"
#include "core/sn_transport_manager.h"
#include "core/sn_port.h"

using std::placeholders::_1;
using std::placeholders::_2;

namespace snode {

CommandSession::CommandSession(RouterImpl *router,
        AddressManager* addressmgr,
        TransportManager* transportmgr,
        const TransEndpoint& ep
)
    :_router(router)
    ,_addressmgr(addressmgr)
    ,_transportmgr(transportmgr)
{
    _cmd_transport = _transportmgr->getUdpTransport(ep);
    _cmd_transport->listenOnRecv(std::bind(&CommandSession::onRecvCmd, this, _1, _2));

    _cmd_parser.setDispatcher(
        CmdDispatcher<RegisterCmd>{std::bind(&CommandSession::onRegister, this, _1)}
    );    
    _cmd_parser.setDispatcher(
        CmdDispatcher<AddressConfirmCmd>{std::bind(&CommandSession::onAddressConfirm, this, _1)}
    );    
    _cmd_parser.setDispatcher(
        CmdDispatcher<ShareRoutingCmd>{std::bind(&CommandSession::onSharedRouting, this,_1)}
    );    
    _cmd_parser.setDispatcher(
        CmdDispatcher<OKCmd>{std::bind(&CommandSession::onCmdOK, this,_1)}
    );    
}

void CommandSession::onRecvCmd(const void *data, int size)
{
    std::cout<<"Recv cmd:";
    std::cout<<(const char*)data<<endl;

    cmd_ptr req = std::move(_cmd_parser.parse((const char*)data, size));
    cmd_ptr rsp = req->dispatch();

    if(rsp != nullptr){
        CommandEncoder* cmde = rsp->encoder();
        _cmd_transport->send(cmde->buf(), cmde->size());
        std::cout<<"send rsp:"<<(const char*)cmde->buf()<<endl;
    }
}

class RegisterTransaction : public Transaction
{
public:
    RegisterTransaction(const std::string& id)
        :Transaction(id)
    {
    }

    void addressAssigned(port_ptr port, const Address& addr){
        _alloced_addr = addr;
        _port = port;
    }

    void setRemoteEndpoint(const TransEndpoint& ep){
        _remote_ep = ep;
    }

    RouterImpl* router=nullptr;
    AddressManager* addressmgr=nullptr;

    void commit()override{
        assert(router);
        assert(addressmgr);
        static_cast<NetTransport*>(_port->transport())->remote_ep(_remote_ep);
	    router->addPort(_port);
	    router->addRouting(_alloced_addr, 1, _alloced_addr, _port);
    }

    void rollback()override{
        addressmgr->releaseAddress(_alloced_addr);
    }

private:
    Address _alloced_addr; 
    TransEndpoint _remote_ep; 
    port_ptr _port;

};

cmd_ptr CommandSession::onRegister(Command* req)
{
    std::shared_ptr<RegisterTransaction> rtransaction(new RegisterTransaction(req->id));
    rtransaction->router = _router;
    rtransaction->addressmgr = _addressmgr;

    Transport* udp = _transportmgr->getUdpTransport(_cmd_transport->local_ep().ip);

    Address addr = _addressmgr->allocAddress();
	port_ptr port = std::make_shared<Port>();
    port->setTransport(udp);

    AddressConfigCmd* cmd = new AddressConfigCmd;
    cmd->id = req->id;
    cmd->address = addr.raw();
    cmd->ip = _cmd_transport->local_ep().ip;
    cmd->port = udp->local_ep().port;

    rtransaction->addressAssigned(port, addr);
    _transaction_map[req->id] = rtransaction;
    return cmd_ptr(cmd);
}


cmd_ptr CommandSession::onAddressConfirm(Command* req)
{
    auto cmd = static_cast<AddressConfirmCmd*>(req);
    TransEndpoint endpoint{cmd->ip, cmd->port};
    auto it = _transaction_map.find(req->id);
    if(it != _transaction_map.end()){
        RegisterTransaction* rtran = static_cast<RegisterTransaction*>(it->second.get());
        rtran->setRemoteEndpoint(endpoint);
        rtran->commit();
        _transaction_map.erase(it);
    }

    OKCmd* rsp = new OKCmd;
    rsp->id = req->id;
    return cmd_ptr(rsp);
}

class SharedRoutingTransaction: public Transaction
{
public:
    SharedRoutingTransaction(const std::string& id)
        :Transaction(id)
    {
    }

    void updateRouting(const Neighbor& neigh, const RoutingInfo& info){
        _neigh = neigh;
        _info = info;
    }

    RoutingMethod* routing_method=nullptr;
    void commit()override{
        routing_method->updateRouting(_neigh.addr, _info);
    }

    void rollback()override{
    }

private:
    Neighbor _neigh;
    RoutingInfo _info;
};

cmd_ptr CommandSession::onSharedRouting(Command* req)
{
    auto cmd = static_cast<ShareRoutingCmd*>(req);

    SharedRoutingTransaction transaction(req->id);
    transaction.routing_method = _router->routing_method();
    transaction.updateRouting(cmd->neighbor, cmd->info);
    transaction.commit();

    return nullptr;
}



cmd_ptr CommandSession::onCmdOK(Command* req)
{
    auto it = _transaction_map.find(req->id);
    if(it != _transaction_map.end()){
        it->second->commit();
        _transaction_map.erase(it);
    }

    return nullptr;
}


}//namespace snode

