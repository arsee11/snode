///sn_neighbor.cpp

#include "sn_neighbor.h"

namespace snode {


void readNeighbors(snode::NeighborMap &ns)
{
    std::ifstream fs("neighbors.conf");
    if( !fs.good()){
        std::cout<<"no neighbors configs\n";
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

    try {
        auto j = jo["neighbors"];
        auto ja = jo["neighbors"].as_array();
        for(auto i : ja){
            auto jneib = i.as_object();
            uint32_t sn = jneib["sn"].as_int64();
            uint32_t en = jneib["en"].as_int64();
            uint16_t lport = jneib["lport"].as_int64();;
            uint16_t rport = jneib["rport"].as_int64();;
            std::string rip= jneib["rip"].as_string().c_str();
            ns.push_back( {Address(sn, en), lport, {rip, rport}} );
        }
    } catch (std::exception& e) {
        std::cout<<"parse neighbors.conf failed:"<<e.what()<<std::endl;
    }
}

}//namespace snode

