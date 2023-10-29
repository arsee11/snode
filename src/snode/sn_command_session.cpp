//sn_command_session.cpp

#include "sn_command_session.h"
#include "core/sn_address_manager.h"
#include "core/sn_transport_manager.h"
#include "core/sn_port.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace snode {

CommandSession::CommandSession(Snode *snode,
        TransportManager* transportmgr,
        const TransEndpoint& ep
)
    :_snode(snode)
    ,_transportmgr(transportmgr)
{
    _cmd_transport_server = _transportmgr->getUdpTransportServer(ep);
    _cmd_transport_server->listenOnConnect(std::bind(&CommandSession::onNewConnect, this, _1));

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
        CmdDispatcher<HelloCmd>{std::bind(&CommandSession::onHello, this,_1)}
    );      
    _cmd_parser.setDispatcher(
        CmdDispatcher<OKCmd>{std::bind(&CommandSession::onCmdOK, this,_1)}
    );    
}

void CommandSession::sayHello(const TransEndpoint &to, const HelloCmd::Hello &h)
{
    std::unique_ptr<Transport> cmd_transport;
    try{
        cmd_transport.reset( _transportmgr->getUdpTransport(_cmd_transport_server->local_ep().ip) );
        cmd_transport->remote_ep(to);
    }catch(std::exception& e){
        return;
    }

    HelloCmd req;
    req.id = Command::genId();
    req.hello = h;
    auto cmde = req.encoder();
    //cout<<"send cmd:"<< t->remote_ep()<<":"<<(const char*)cmde->buf()<<endl;
    cmd_transport->send(cmde->buf(), cmde->size());
}

void CommandSession::onNewConnect(const trans_ptr& conn)
{
    conn->listenOnRecv([this, conn](const void* data, int size){
        this->onRecvCmd(data, size, conn);
    });

    _cmd_transports.push_back(conn);
}

void CommandSession::onRecvCmd(const void *data, int size, trans_ptr t)
{
    //std::cout<<"Recv cmd from "<<t->remote_ep()<<":";
    //std::cout<<std::string((const char*)data, size)<<endl;

    cmd_ptr req = std::move(_cmd_parser.parse((const char*)data, size));
    if(req == nullptr){
        return;
    }
    cmd_ptr rsp = req->dispatch();

    if(rsp != nullptr){
        CommandEncoder* cmde = rsp->encoder();
        t->send(cmde->buf(), cmde->size());
        //std::cout<<"send rsp to "<<t->remote_ep()<<":";
        //std::cout<<(const char*)cmde->buf()<<endl;
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

    Snode* snode=nullptr;
    AddressManager* addressmgr=nullptr;

    void commit()override{
        assert(snode);
        snode->addEnode(_alloced_addr, _port, _remote_ep);
    }

    void rollback()override{
        snode->releaseAddress(_alloced_addr);
    }

private:
    Address _alloced_addr; 
    TransEndpoint _remote_ep; 
    port_ptr _port;

};

cmd_ptr CommandSession::onRegister(Command* req)
{
    std::shared_ptr<RegisterTransaction> rtransaction(new RegisterTransaction(req->id));
    rtransaction->snode = _snode;
    port_ptr port = _snode->newPort();
    Address addr = _snode->allocAddress();
	

    AddressConfigCmd* cmd = new AddressConfigCmd;
    cmd->id = req->id;
    cmd->address = addr.raw();
    cmd->ip = _cmd_transport_server->local_ep().ip;
    cmd->port = port->transport()->local_ep().port;

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
    //transaction.routing_method = _router->routing_method();
    transaction.updateRouting(cmd->neighbor, cmd->info);
    transaction.commit();

    return nullptr;
}

cmd_ptr CommandSession::onHello(Command* req)
{
    auto cmd = static_cast<HelloCmd*>(req);
    Address addr = cmd->hello.address;
    TransEndpoint ep(cmd->hello.ip, cmd->hello.port);

    _snode->configNeighbor(addr, ep);
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

