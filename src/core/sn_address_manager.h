///sn_address_manager.h

#ifndef SN_ADDRESS_MANAGER_H
#define SN_ADDRESS_MANAGER_H

#include "sn_address.h" 
#include <bitset> 

namespace snode
{

struct InvalidAddressException
{
};

struct AddressFullException
{
};

class AddressManager
{
    static constexpr uint32_t MAX=1024;

public:
    AddressManager(uint32_t snaddr);

    Address allocAddress();
    void releaseAddress(const Address& addr);

private:
    uint32_t getAddress();
    uint32_t getTotalAlloced();

private:
    uint32_t _snaddr;
    std::bitset<MAX> _bitset;
};

}

#endif /*SN_ADDRESS_MANAGER_H*/
