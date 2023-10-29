///sn_address_manager.cpp

#include "sn_address_manager.h" 

namespace snode
{

AddressManager::AddressManager(uint32_t snaddr)
    :_snaddr(snaddr)
{
    _bitset.set(0);
}

Address AddressManager::allocAddress()
{
    if(getTotalAlloced() == MAX){
        throw AddressFullException();
    }

    uint64_t en = getAddress();

    return Address(_snaddr, en);
}

void AddressManager::releaseAddress(const Address& addr)
{
    size_t i = addr.en();
    if(i >= MAX ){
        throw InvalidAddressException();
    }
   
    _bitset.reset(i-1);
}


uint32_t AddressManager::getAddress()
{
    uint32_t addr=0;
    for(size_t i=0; i<_bitset.size(); i++){
        if( !_bitset.test(i) ){
            addr = i+1;
            _bitset.set(i);
            break;
        }
    }

    return addr;
}

uint32_t AddressManager::getTotalAlloced()
{
    return _bitset.count();
}

}//namespace snode

