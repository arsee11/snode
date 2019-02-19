///snode_test.cpp

#include "../src/sn_transport_manager.h"
#include "../src/sn_port.h"
#include "../src/sn_message.h"
#include "../src/sn_router.h"
#include "../src/sn_route_table.h"
#include "../src/sn_rip_routing.h"

using namespace snode;

#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	TransportManager transportmgr;

	Router<RIPRouting, RouteTable> r;
	
	UdpTransport* udp= transportmgr.getUdpTransport();

	TransEndpoint lep1={10010, "127.0.0.1"};
	TransEndpoint rep1={10011, "127.0.0.1"};
	port_ptr up1 = std::make_shared<UdpPort>(udp, lep1, rep1);
	r.addPort(up1);

	Address sn_addr1(1ul, 0ul);
	Address addr1(1ul, 1ul);
	r.addRouting(sn_addr1, addr1, 1, up1);

	TransEndpoint lep2={10010, "127.0.0.1"};
	TransEndpoint rep2={10012, "127.0.0.1"};
	port_ptr up2 = std::make_shared<UdpPort>(udp, lep2, rep2);
	r.addPort(up2);

	Address sn_addr2(2ul, 0ul);
	Address addr2(2l, 1ul);
	r.addRouting(sn_addr2, addr2, 1, up2);

	while(true)
	{
		transportmgr.run();
	}


	return 0;
}
