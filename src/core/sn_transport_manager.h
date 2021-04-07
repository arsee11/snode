#ifndef SN_TRANSPORT_MANAGER_H
#define SN_TRANSPORT_MANAGER_H

#include "sn_transport.h"
#include "sn_udptransport.h"
#include "sn_threading.h"

namespace snode {

class TransportManager
{
public:
    TransportManager();
    void clear();
    UdpTransport* getUdpTransport(const TransEndpoint& local);
    UdpTransport* getUdpTransport(const std::string& lip);

private:
    void evtLoop();

private:
    NetEventQueue _evt_queue;
    ThreadScope _thread;
    bool _is_running=true;

};

}//namespace snode

#endif // SN_TRANSPORT_MANAGER_H
