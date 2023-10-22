///snode_test.cpp

#include <iostream>
#include "enode/sn_enode_impl.h"

using namespace snode;
using namespace std;
using namespace std::placeholders;

void onInput(const Message& msg)
{
	cout<<"recv msg from["<<msg.src().sn()<<","<<msg.src().en()
        <<"] payload:"<<(char*)msg.payload()<<endl;
}


int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		cout<<"usage: [local ip] [snode ip] [snode port]"<<endl;
		return 0;
	}

	const char* lip = argv[1];
	const char* ip = argv[2];
	uint16_t port= atoi(argv[3]);
    TransEndpoint snode_ep{ip, port};

    ENodeImpl node(lip);
    node.listenOnRecv(&onInput);
    node.connect(snode_ep, [snode_ep, &node](int ec, const char* msg){
        if(ec!=0){
            cout<<"connecto snode addr["<< snode_ep.ip<<":"<<snode_ep.port<<"] failed\n";
            exit(1);
        }
        cout<<"connecto snode addr["<< snode_ep.ip<<":"<<snode_ep.port<<"] success\n";
        cout <<"my addr is [0x"<<hex<<node.address().raw()<<std::dec<<"]\n";
    });
    

    while(true){
        cout<<"input [sn] [en] [mesage]:\n";
        uint32_t snode_addr=-1;
        uint32_t enode_addr=-1;
        std::string m;
        cin>>snode_addr>>enode_addr>>m;

        Address peer_addr(snode_addr, enode_addr);
        int n = node.sendTo(m.c_str(), m.size(), peer_addr); 
        cout<<"send to ["<<snode_addr<<":"<<enode_addr<<","<<m<<"],"<<n<<" bytes.\n";
    }

	return 0;
}
