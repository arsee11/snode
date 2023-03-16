///sn_test.cpp

#include "core/sn_transport_manager.h"
#include "core/sn_address_manager.h"
#include "core/sn_json_route_serializer.h"
#include "snode/sn_command_session.h"
#include "snode/sn_snode_impl.h"
#include "snode/sn_neighbor.h"

using namespace snode;

#include <iostream>
#include <fstream>
using namespace std;
using namespace std::placeholders;

TransportManager transportmgr;
RouterImpl router;

std::string local_ip="127.0.0.1";

void dumpRoute()
{
    cout<<"------------routes-----------------\n";
    JsonRouteSerializer<RouterImpl> s(&router);
    cout<<s()<<endl;
}

void configStaticRoute(Snode* sn)
{
    std::ifstream fs("static_routes.conf");
    if( !fs.good()){
        std::cout<<"no static route configs\n";
        return;
    }

    boost::json::error_code ec;
    boost::json::stream_parser p;
    std::string line;
    while( std::getline( fs, line ) )
    {
        p.write( line, ec );
        if( ec ){
            std::cout<<"configs syntax error\n";
            return ;
        }
    }

    p.finish( ec );
    if( ec ){
        std::cout<<"configs syntax error\n";
        return ;
    }
    auto jo = p.release().as_object();

    auto ja = jo["routes"].as_array();
    for(auto i : ja){
        auto jneib = i.as_object();
        uint32_t snaddr = jneib["sn"].as_int64();
        uint32_t en = jneib["en"].as_int64();
        uint32_t nsn = jneib["nsn"].as_int64();
        uint32_t nen = jneib["nen"].as_int64();
        uint32_t metric = jneib["metric"].as_int64();
        sn->addStaticRoute(Address(snaddr,en), metric, Address(nsn,nen));
    }
}

void setNeighbors(Snode* sn, const NeighborMap& ns)
{
    for(auto& neib : ns){
		sn->setNeighbor(neib);
    }
}


int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		cout<<"usage: [sn addr] [local ip] [local port]"<<endl;
		return 0;
	}

	AddressManager addressmgr = AddressManager(atoi(argv[1]));
	SnodeImpl sn(&router, &addressmgr, &transportmgr);
	local_ip = argv[2];
	uint16_t port= atoi(argv[3]);
    TransEndpoint service_ep{local_ip, port};

    NeighborMap neighbours;
    readNeighbors(neighbours);
    setNeighbors(&sn, neighbours);
    configStaticRoute(&sn);

	CommandSession cmd_sess(&router,
			&addressmgr,
			&transportmgr,
			service_ep);

    char ch;
    while(ch != 'q'){
        cout<<">>";
        cin >> ch;
        if(ch == 'd'){
            dumpRoute();
        }
    }

    transportmgr.clear();

	return 0;
}
