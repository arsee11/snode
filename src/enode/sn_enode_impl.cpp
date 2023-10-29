///sn_enode_impl.cpp

#include "sn_enode_impl.h"


using namespace std::placeholders;

namespace snode {

ENodeImpl::ENodeImpl(const std::string& lip)
    :_local_ip(lip)
{
    _transportmgr.reset(new TransportManager());
    _cmd_session.reset( new enode::CommandSession(this, _transportmgr.get()));
}

void ENodeImpl::listenOnRecv(RecvCb cb)
{
    _recv_cb = cb;
}

void ENodeImpl::connect(const TransEndpoint& snode_ep, ConnectCb cb)
{
    _conn_cb = cb;
    if( !_cmd_session->open(TransEndpoint{_local_ip, 0}, snode_ep)){
    //TODO:handle errors
    }
}

int ENodeImpl::sendTo(const void* data, int len, const Address& peer)
{
    if( _data_transport == nullptr){
        std::cout<<"Not connect to any SNode yet."<<std::endl;
        return 0;
    }
    Message msg(_addr, peer, data, len);
    uint8_t* buf=nullptr;
    size_t size=0;
    std::tie(buf, size) = msg.encode();
    return _data_transport->send(buf, size);
}

static bool check_port(uint16_t port){
    return port > 0;
}

TransEndpoint ENodeImpl::configAddress(const Address &addr, const TransEndpoint& remote_ep)
{
    try{
        _data_transport.reset( _transportmgr->getUdpTransport(_local_ip) );
    }catch(std::exception& e){
        //return ERRORCmd("data transport open failed");
    }
    if( !check_port(remote_ep.port)){
        //return ERRORCmd("endpoint port is invalid.");
        std::cout<<"remote endpoint port["<<remote_ep.port<<"] is invalid."<<std::endl;
    }
    _data_transport->remote_ep(remote_ep);
    _data_transport->listenOnRecv([this](const void* data, int size){
        //ToDo: verify rempte_ep

        Message lmsg;
        lmsg.decode((const uint8_t*)data, size);
        if(lmsg.dst() == _addr){
            if(_recv_cb != nullptr){
                _recv_cb(lmsg);
            }
        }
        else{
            //log not my message
        }
    });
    _addr = addr;

     auto localep = _data_transport->local_ep();
     if(_conn_cb != nullptr){
         _conn_cb(0, "");
     }

     return localep;
}

}//namespace snode
