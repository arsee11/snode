///sn_test.cpp

#include <fstream>
#include <boost/json.hpp>

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
    cout<<"------------route table-----------------\n";
    // JsonRouteSerializer<RouterImpl> s(router);
    // cout<<s()<<endl;

    cout<<"dst\t mask\t metric\t next\t lep\t\t\t rep"<<endl;

    if(router != nullptr){
        auto items = router->route_table().getAllItems();
        for(size_t i=0; i<items.size(); i++){
            auto item = items[i];
            cout<<item->dst <<"\t "<<item->mask<<"\t "<<item->metric<<"\t "<<item->next_hop<<"\t "
                <<item->port->transport()->local_ep().ip<<":"<<item->port->transport()->local_ep().port<<"\t "
                <<item->port->transport()->remote_ep().ip<<":"<<item->port->transport()->remote_ep().port<<endl;

        }
    }

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

NeighborList readNeighbors()
{
    NeighborList ns;
    std::ifstream fs("neighbors.conf");
    if( !fs.good()){
        std::cout<<"no neighbors configs\n";
        return ns;
    }

    boost::json::error_code ec;
    boost::json::stream_parser p;
    std::string line;
    while( std::getline( fs, line ) )
    {
        p.write( line, ec );
        if( ec ){
            std::cout<<"neighbors.conf configs syntax error\n";
            return ns;
        }
    }

    p.finish( ec );
    if( ec ){
        std::cout<<"neighbors.conf configs syntax error\n";
        return ns;
    }
    auto jo = p.release().as_object();

    try {
        auto j = jo["neighbors"];
        auto ja = jo["neighbors"].as_array();
        for(auto i : ja){
            auto jneib = i.as_object();
            uint32_t sn = jneib["sn"].as_int64();
            uint32_t en = jneib["en"].as_int64();
            std::string name= jneib["name"].as_string().c_str();
            uint16_t port = jneib["port"].as_int64();;
            std::string ip= jneib["ip"].as_string().c_str();
            ns.push_back( {name, Address(sn,en), nullptr, {ip, port}} );
        }
    } catch (std::exception& e) {
        std::cout<<"parse neighbors.conf failed:"<<e.what()<<std::endl;
    }

    return ns;
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

    EventQueueEpoll eq;
    CurThreadingScope curc(&eq);
    NeighborManagerT<CurThreadingScope> nm(&sn, &curc);

	local_ip = argv[2];
	uint16_t port= atoi(argv[3]);
    TransEndpoint service_ep{local_ip, port};
	CommandSession cmd_sess(&sn,
            &nm,
			&transportmgr,
			service_ep);

    configStaticRoute(&sn);

    NeighborList neighbours = readNeighbors();    
    nm.initNeighbors(&cmd_sess, neighbours);

    //listen stdin (fd=0)
    eq.bindInput(0, [&]{
        cmd(router, &curc);
    });

    curc.run();

    transportmgr.clear();

	return 0;
}
