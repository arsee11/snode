﻿///sn_routing.h

#ifndef SN_ROUTING_H
#define SN_ROUTING_H

#include "core/sn_address.h"
#include <vector>

namespace snode {

struct RoutingInfo
{
    struct Item{
        Address dst;        //destination snode/enode address
	    uint64_t mask;      //snode adderss mask
	    int metric;         //metric of this route path
    };

    std::vector<Item> items;
};

class RoutingMethod
{
public:
    virtual ~RoutingMethod(){}
    virtual void updateRouting(const Address& from, const RoutingInfo& info)=0;
};

}//namespace snode

#endif/*SN_ROUTING_H*/
