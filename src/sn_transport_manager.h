#ifndef SN_TRANSPORT_MANAGER_H
#define SN_TRANSPORT_MANAGER_H

#include "sn_udptransport.h"
#include <easynet/netevent_queue.h>

namespace snode {

class TransportManager
{
public:
    TransportManager();
    UdpTransport* getUdpTransport(){ return _udpt; }
    void run();
    void stop(){
        _udpt->close();
    }

private:
    NetEventQueue _evt_queue;
    UdpTransport* _udpt=nullptr;

};

}//namespace snode

#endif // SN_TRANSPORT_MANAGER_H
