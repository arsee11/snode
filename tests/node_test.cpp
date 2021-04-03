///snode_test.cpp

#include <iostream>
//#include "node.h"
#include "sn_transport.h"
#include "sn_transport_manager.h"
#include "sn_address.h"
#include "sn_message.h"
#include "commands.h"

using namespace snode;
using namespace std;
using namespace std::placeholders;

TransportManager transm;

class Node
{
    using RecvCb = std::function<void(const Message& msg)>;
    using ConnectCb = std::function<void(int ec, const char* msg)>;

public:
    Node();
    void listenOnRecv(RecvCb cb);
    void connect(const TransEndpoint& snode_ep, ConnectCb cb);
    void sendTo(const void* data, int len, const Address& peer);
    Address address()const{ return _addr; }

private:
    cmd_ptr onConfigAddress(Command* req);
    cmd_ptr onCmdOK(Command* req);

private:
    RecvCb _recv_cb;
    ConnectCb _conn_cb;
    Address _addr;
    std::unique_ptr<NetTransport> _cmd_transport;
    std::unique_ptr<NetTransport> _data_transport;
    CommandParser _parser;
};

Node::Node()
{
    _parser.setDispatcher(
        CmdDispatcher<AddressConfigCmd>{std::bind(&Node::onConfigAddress, this, _1)}
    );
    _parser.setDispatcher(
        CmdDispatcher<OKCmd>{std::bind(&Node::onCmdOK, this, _1)}
    );
}

void Node::listenOnRecv(RecvCb cb)
{
    _recv_cb = cb;
}

cmd_ptr Node::onConfigAddress(Command* req)
{
    auto acmd = static_cast<AddressConfigCmd*>(req); 
    _addr = Address(acmd->address);
    TransEndpoint lep{"0.0.0.0", 0};
    try{
        _data_transport.reset( transm.getUdpTransport(lep) );
    }catch(std::exception& e){
        //return ERRORCmd("data transport open failed");
    }
    _data_transport->listenOnRecv([this](const void* data, int size){
        //verify rempte_ep

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
    auto tlep = _data_transport->local_ep();
    auto rsp = new AddressConfirmCmd;
    rsp->id=req->id;
    rsp->ip = tlep.ip;
    rsp->port = tlep.port;
    return cmd_ptr(rsp);
}

cmd_ptr Node::onCmdOK(Command* req)
{
    return nullptr;
}

void Node::connect(const TransEndpoint& snode_ep, ConnectCb cb)
{
    _conn_cb = cb;
    TransEndpoint lep{"0.0.0.0", 0};
    try{
        _cmd_transport.reset( transm.getUdpTransport(lep) );
        _cmd_transport->remote_ep(snode_ep);
    }catch(std::exception& e){
        //_conn_cb();
        return;
    }
    
    int id=0;
    RegisterCmd req;
    req.id = std::to_string(id);
    auto cmde = req.encoder();
    cout<<"send cmd:"<<(char*)cmde->buf()<<endl;
    _cmd_transport->send(cmde->buf(), cmde->size());
    _cmd_transport->listenOnRecv([this](const void* data, int size){
            cout<<"recv cmd:"<<(char*)data<<endl;
            cmd_ptr req = std::move(_parser.parse((const char*)data, size));
            cmd_ptr rsp = std::move(req->dispatch());
            if(rsp != nullptr){
                auto cmde = rsp->encoder();
                cout<<"send cmd:"<<(char*)cmde->buf()<<endl;
                _cmd_transport->send(cmde->buf(), cmde->size());
            }

    });

    //TODO:handle errors

}

void Node::sendTo(const void* data, int len, const Address& peer)
{
    Message msg(_addr, peer, data, len);
    uint8_t* buf=nullptr;
    size_t size=0;
    std::tie(buf, size) = msg.encode();
    _data_transport->send(buf, size);
}




void onInput(const Message& msg)
{
	cout<<"recv msg("<<msg.size()<<") payload:"<<(char*)msg.payload()<<endl;
}


int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"usage: [snode ip] [snode port]"<<endl;
		return 0;
	}

	const char* ip = argv[1];
	uint16_t port= atoi(argv[2]);
    TransEndpoint snode_ep{ip, port};

    Node node;
    node.listenOnRecv(&onInput);
    node.connect(snode_ep, [snode_ep](int ec, const char* msg){
        if(ec!=0){
            cout<<"connecto snode addr["<< snode_ep.ip<<":"<<snode_ep.port<<"] failed\n";
            exit(1);
        }
    });
    
    cout<<"connecto snode addr["<< snode_ep.ip<<":"<<snode_ep.port<<"] success\n";
    cout <<"my addr is ["<<node.address().raw()<<"]\n";

    cout<<"input [addr] [mesage]\n";
    uint64_t node_addr=-1;
    std::string m;
    cin>>node_addr>>m;

    int64_t sn_addr = node.address().sn();
    Address peer_addr(sn_addr, node_addr);
    node.sendTo(m.c_str(), m.size(), peer_addr); 

    getchar();

	return 0;
}
