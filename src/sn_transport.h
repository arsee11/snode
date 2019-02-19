#ifndef SN_TRANSPORT_H
#define SN_TRANSPORT_H

#include <easynet/addr.h>
#include <easynet/udppeer.h>
#include <easynet/netlisteners.h>

namespace snode {

    using TransEndpoint = arsee::net::AddrPair;
    using UdpPeer = arsee::net::UdpPeer;
    using NetMsg = arsee::net::MsgSt;
    using UdpInpuListner = arsee::net::UdpInputListener;
    using NetEventQueue = arsee::net::NetEventQueue;

}//namespace node

#endif // SN_TRANSPORT_H
