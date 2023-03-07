//sn_command_session.cpp

#include "sn_command_session.h"
#include "core/sn_address_manager.h"
#include "core/sn_transport_manager.h"
#include "core/sn_port.h"

using std::placeholders::_1;
using std::placeholders::_2;

namespace snode {

namespace enode {

CommandSession::CommandSession(ENode *en, TransportManager *transportmgr)
    :_enode(en)
    ,_transportmgr(transportmgr)

{
    _cmd_parser.setDispatcher(
        CmdDispatcher<AddressConfigCmd>{std::bind(&CommandSession::onConfigAddress, this, _1)}
    );
    _cmd_parser.setDispatcher(
        CmdDispatcher<OKCmd>{std::bind(&CommandSession::onCmdOK, this, _1)}
    );
}

bool CommandSession::open(const TransEndpoint& local_ep, const TransEndpoint& snode_ep)
{
    try{
        _cmd_transport.reset( _transportmgr->getUdpTransport(local_ep) );
        _cmd_transport->listenOnRecv(std::bind(&CommandSession::onRecvCmd, this, _1, _2));
        _cmd_transport->remote_ep(snode_ep);
    }catch(std::exception& e){
        //_conn_cb();
        return false;
    }

    RegisterCmd req;
    req.id = Command::genId();
    auto cmde = req.encoder();
    cout<<"send cmd:"<<(const char*)cmde->buf()<<endl;
    _cmd_transport->send(cmde->buf(), cmde->size());

    //TODO:handle errors
    return true;
}

void CommandSession::onRecvCmd(const void *data, int size)
{
    std::cout<<"Recv cmd:";
    std::cout<<(const char*)data<<endl;

    cmd_ptr req = std::move(_cmd_parser.parse((const char*)data, size));
    if(req == nullptr){
        return;
    }
    cmd_ptr rsp = req->dispatch();

    if(rsp != nullptr){
        CommandEncoder* cmde = rsp->encoder();
        _cmd_transport->send(cmde->buf(), cmde->size());
        std::cout<<"send rsp:"<<(const char*)cmde->buf()<<endl;
    }
}

cmd_ptr CommandSession::onConfigAddress(Command* req)
{
    auto cmd = static_cast<AddressConfigCmd*>(req);
    TransEndpoint tlep = _enode->configAddress( Address(cmd->address), TransEndpoint{cmd->ip, cmd->port});


    auto rsp = new AddressConfirmCmd;
    rsp->id=req->id;
    rsp->ip = tlep.ip;
    rsp->port = tlep.port;

    return cmd_ptr(rsp);
}

cmd_ptr CommandSession::onCmdOK(Command* req)
{
    return nullptr;
}




}//namespace enode

}//namespace snode

