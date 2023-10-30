///sn_test.cpp

#include "core/sn_transport_manager.h"
#include "core/sn_address_manager.h"
#include "core/sn_json_route_serializer.h"
#include "snode/sn_command_session.h"
#include "snode/sn_snode_impl.h"
#include "snode/sn_rip_routing.h"
#include "snode/sn_neighbor.h"
#include "core/sn_timer.h"

using namespace snode;

#include <iostream>
#include <fstream>
using namespace std;
using namespace std::placeholders;


std::string local_ip="127.0.0.1";

void dumpRoute(RouterImpl* router)
{
    cout<<"------------routes-----------------\n";
    JsonRouteSerializer<RouterImpl> s(router);
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

void setNeighbors(CommandSession* ss, Snode* sn, NeighborMap& ns)
{
    for(auto& neib : ns){
		port_ptr port = sn->newPort();
        neib.port = port;
        sn->addNeighbor(neib);
    
        HelloCmd::Hello hello{
            sn->getAddress().raw(),
            port->transport()->local_ep().ip,
            port->transport()->local_ep().port
        };
		ss->sayHello(neib.comminucate_ep, hello);
    }
}

void updateNeighbors(CommandSession* ss, Snode* sn)
{
    auto ns = sn->getNeighbors();
    for(auto& neib : ns){
    
        HelloCmd::Hello hello{
            sn->getAddress().raw(),
            neib.port->transport()->local_ep().ip,
            neib.port->transport()->local_ep().port
        };
		ss->sayHello(neib.comminucate_ep, hello);
    }
}

void cmd(RouterImpl* router, CurThreadingScope* s)
{
    char ch;

    cout<<">>";
    cin >> ch;
    if(ch == 'q'){
        s->stop();
        return;
    }
    if(ch == 'd'){
        dumpRoute(router);
    }
}


int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		cout<<"usage: [sn addr] [local ip] [local port]"<<endl;
		return 0;
	}

    RouterImpl* router = new RouterImpl(new RIPRoutingMethod);
    TransportManager transportmgr;
	AddressManager addressmgr = AddressManager(atoi(argv[1]));
	SnodeImpl sn(router, &addressmgr, &transportmgr);

	local_ip = argv[2];
	uint16_t port= atoi(argv[3]);
    TransEndpoint service_ep{local_ip, port};
	CommandSession cmd_sess(&sn,
			&transportmgr,
			service_ep);

    configStaticRoute(&sn);

    NeighborMap neighbours;
    readNeighbors(neighbours);
    setNeighbors(&cmd_sess, &sn, neighbours);

    EventQueueEpoll eq;
    CurThreadingScope curc(&eq);
    std::unique_ptr<Timer> timer(Timer::start(3000, [&]{
        updateNeighbors(&cmd_sess, &sn);
        cout<<"update neighbors...\n";
    }, curc.poller()));

    //listen stdin (fd=0)
    eq.bindInput(0, [&]{
        cmd(router, &curc);
    });

    curc.run();

    transportmgr.clear();

	return 0;
}
