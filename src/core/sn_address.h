///sn_address.h

#ifndef SN_ADDRESS_H
#define SN_ADDRESS_H

#include <cstdint>
#include <functional>
#include <assert.h>
#include <endian.h>

namespace snode
{

///high                                          low
///**************************************************
///|super node addr:24 bits|endian node addr:40 bits|
///**************************************************
class Address
{
public:
    typedef uint64_t raw_t;
	static constexpr uint64_t SN_MAX=0x0000000000ffffff;
	static constexpr uint64_t EN_MAX=0x000000ffffffffff;

private:
	struct addr_t{
        //little endian
		uint32_t en;
		uint32_t sn;
	};

public:
	Address(uint32_t sn, uint32_t en){
		assert(sn<=SN_MAX);
		assert(en<=EN_MAX);

		_addr.sn = sn;
		_addr.en = en;
	}

    Address(raw_t raw=0, bool is_network_order=false){
        if(is_network_order)
        {
            raw_t tmp = htobe64(raw);
            _addr = *(addr_t*)&tmp;
        }
        else
            _addr = *(addr_t*)&raw;

	}

    size_t size()const{ return sizeof _addr;}

	///super node address
	uint32_t sn()const{ return _addr.sn; }
	
	///endian node address
    uint32_t en()const{ return _addr.en; }

	///super node mask
    raw_t mask();

    raw_t raw()const{
        return *(raw_t*)&_addr;
	}
	
    //to network byte order
    raw_t rawton()const{ return htobe64(raw()); }

	bool operator==(const Address& rhs)const{
		return raw() == rhs.raw();
	}

	
	bool operator!=(const Address& rhs)const{
		return !((*this) == rhs ); 
	}

	bool operator<(const Address& rhs)const{
		return raw() < rhs.raw();
	}

private:
	static constexpr uint64_t SN_MASK=0xffffffff00000000;
	static constexpr uint64_t EN_MASK=0x00000000ffffffff;

	addr_t _addr;
};


struct AddressHash
{
	inline size_t operator()(const Address& addr)const{
        return std::hash<uint64_t>{}(addr.raw());
	}
};

}

#endif /*SN_ADDRESS_H*/
