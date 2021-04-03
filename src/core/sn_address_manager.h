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
    static constexpr uint64_t MAX=1024;

public:
    AddressManager(const Address& base);

    Address allocAddress();
    void releaseAddress(const Address& addr);

private:
    uint64_t getAddress();
    uint64_t getTotalAlloced();

private:
    Address _base;
    std::bitset<MAX> _bitset;
};

}

#endif /*SN_ADDRESS_MANAGER_H*/
