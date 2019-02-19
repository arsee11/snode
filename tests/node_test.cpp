///snode_test.cpp

#include <iostream>
#include "../src/sn_transport_manager.h"
#include "../src/sn_port.h"
#include "../src/sn_message.h"
#include <thread>


using namespace snode;
using namespace std;

void onInput(Port* port, const Message& msg)
{
	cout<<"recv msg("<<msg.size()<<") payload:"<<(char*)msg.payload()<<endl;
}


int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout<<"usage: node 1/2"<<endl;
		return 0;
	}

	uint64_t src = atol(argv[1]);
	uint64_t dst = src==1?2:1;

	TransportManager transportmgr;
	
	TransEndpoint laddr={10010+src, "127.0.0.1"};
	TransEndpoint raddr={10010, "127.0.0.1"};
	UdpTransport* udp = transportmgr.getUdpTransport();
	UdpPort port(udp, laddr, raddr);
	port.setInputCallBack(onInput);

	bool isrun=true;
	thread t([&transportmgr, &isrun](){ 
		while(isrun){
			transportmgr.run();
		}
	} );

	Address dest_addr(dst, dst);
	Address local_addr(src, src);
	while(true)
	{
		cout<<"input a message(q to exit):";
		std::string str;
		cin>>str;
		if(str == "q")
			break;

		Message msg(local_addr, dest_addr, str.c_str(), str.size());
		port.output(msg);
	}

	isrun = false;
	transportmgr.stop();
	cout<<"t.join()"<<endl;
	t.join();

	return 0;
}
