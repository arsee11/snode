//address_test.cpp

#include "../src/sn_address.h"
#include <endian.h>

using namespace snode;

int main()
{
	Address addr(0x1000ul, 0x12345ul);
	assert(addr.size()==64/8);
	assert(addr.sn()==0x1000ul);
	assert(addr.en()==0x12345ul);

	Address addr2(0x1001ul, 0x12345ul);
	assert(addr != addr2);

	Address addr3(0x1000ul, 0x12346ul);
	assert(addr != addr3);
	assert(addr < addr3);


	Address addr4(0x1000ul, 0x12345ul);
	assert(addr == addr4);

	uint64_t raw = 0x1000ul<<40 | 0x12345ul;
	assert(addr4.raw() == raw);

	Address addr5(addr4.raw());
	assert(addr5 == addr4);

	Address addr6 = addr4.raw();
	assert(addr6 == addr4);
	
	uint64_t rawn=htobe64(raw);
	Address addr7(rawn, true);
	assert(addr7 == addr4);

	return 0;
}
