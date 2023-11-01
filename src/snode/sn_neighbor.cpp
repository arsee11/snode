///sn_neighbor.cpp

#include "sn_neighbor.h"
#include "sn_command_session.h"

namespace snode {


void NeighborManager::initNeighbors(CommandSession* ss, const NeighborList& ns)
{
    for(const auto& neib : ns){
        addNeighbor(neib);
    }

    for(auto nptr : _neighbors){
        keepalive(nptr.second);
        helloToNeighbor(ss, nptr.second);
    }

}

AwaitTimer::co_task NeighborManager::helloToNeighbor(CommandSession* ss, neighbor_ptr neib)
{
    while(true){            
        std::cout<<"say hello to "<<neib->neib.addr<<std::endl; 
        HelloCmd::Hello hello{
            _sn->getAddress().raw(),
            neib->neib.port->transport()->local_ep().ip,
            neib->neib.port->transport()->local_ep().port
        };
        ss->sayHello(neib->neib.comminucate_ep, hello);

        co_await neib->hello_timer->await(3000);   
        if(neib->disable){
            break;
        }    
    }
}



AwaitTimer::co_task NeighborManager::keepalive(neighbor_ptr neib)
{    
    while(true){
        int event = co_await neib->keepalive_timer->await(10000);
        if(neib->disable){
            break;
        }
        std::cout<<"keepalive for "<<neib->neib.addr<<"event:"<<event<<std::endl; 

        if(event == Timer::Timeout){
            disableNeigbor(neib);
            break;
        }
    }
}

void NeighborManager::onNeighborUpdate(CommandSession* ss, const Address& neib_addr, const TransEndpoint& msg_ep)
{
    auto neib = _neighbors[neib_addr];
    if(neib != nullptr){
        if( neib->disable ){
            neib->disable = false;
            _sn->configNeighbor(neib->neib, msg_ep);
            keepalive(neib);
            helloToNeighbor(ss, neib);
        }
        else{
            _sn->configNeighbor(neib->neib, msg_ep);
            neib->keepalive_timer->reset();
        }
    }
    else{
        //log()<<recv a neighbor update, but not my config;
    }
}

void NeighborManager::disableNeigbor(neighbor_ptr neib)
{
    if(neib == nullptr){
        return;
    }
    neib->disable = true;
    _sn->disableNeighbor(neib->neib);
}

}//namespace snode

