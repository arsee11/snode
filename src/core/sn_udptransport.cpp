//sn_udptransport.cpp

#include "sn_udptransport.h"

namespace snode
{

int UdpTransport::send(const void *data, int size){
    if(!is_valid(_remote_ep) ){
        std::cout <<"remote endpoint not set, can't send."<<std::endl;
        return 0;
    }
    if(_udp != nullptr){
        _udp->sendTo(_remote_ep, NetMsg(data, size));
        return size;
    }

    return 0;
}

void UdpTransport::onRecv(const TransEndpoint &remote, const NetMsg &msg){
    if( remote != _remote_ep){
        std::cout <<"From ["<<_remote_ep.ip<<":"<<_remote_ep.port<<":] not my expected. drop it."<<std::endl;
        return;
    }

    if(_recv_cb != nullptr){
        _recv_cb(msg.rd_ptr(), msg.size());
    }
}


bool UdpTransportServer::open()
{
    return true;
}

void UdpTransportServer::onRecv(const TransEndpoint &remote, const NetMsg &msg)
{
    udpcli_ptr cli;
    auto it = _clients.find(remote);
    //if have not received data from 'remote' ever, so it is a  new connection
    if(it == _clients.end()){
        cli = std::make_shared<UdpTransportClient>(_udp, _local_ep);
        cli->remote_ep(remote);
        _clients.insert({remote, cli});
        if(_conn_cb != nullptr){
            _conn_cb(cli);
        }
    }
    else{
        cli = it->second;
    }

    cli->onRecv(msg);
}

UdpTransportServer::UdpTransportClient::UdpTransportClient(const std::shared_ptr<UdpPeer> &udp, const TransEndpoint &local)
{
    _udp = udp;
    _local_ep = local;
}

void UdpTransportServer::UdpTransportClient::onRecv(const NetMsg &msg)
{
    UdpTransport::onRecv(_remote_ep, msg);
}


}//snode
