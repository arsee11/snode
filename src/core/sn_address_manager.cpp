///sn_address_manager.cpp

#include "sn_address_manager.h" 

namespace snode
{

AddressManager::AddressManager(const Address& base)
    :_base(base)
{
}

Address AddressManager::allocAddress()
{
    if(getTotalAlloced() == MAX){
        throw AddressFullException();
    }

    uint64_t en = getAddress();

    return Address(_base.sn(), en);
}

void AddressManager::releaseAddress(const Address& addr)
{
    size_t i = addr.en();
    if(i >= MAX ){
        throw InvalidAddressException();
    }
   
    _bitset.reset(i);
}


uint64_t AddressManager::getAddress()
{
    uint64_t addr=0;
    for(size_t i=0; i<_bitset.size(); i++){
        if( !_bitset.test(i) ){
            addr = i+1;
            _bitset.set(i);
            break;
        }
    }

    return addr;
}

uint64_t AddressManager::getTotalAlloced()
{
    return _bitset.count();
}

}//namespace snode

