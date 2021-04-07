//address_test.cpp

#include "core/sn_address.h"
#include <iostream>

using namespace snode;
using namespace std;

int main()
{
	Address addr(0x1000u, 0x12345u);
	assert(addr.size()==64/8);
	assert(addr.sn()==0x1000);
	assert(addr.en()==0x12345);

	Address addr2(0x1001u, 0x12345u);
	assert(addr != addr2);

	Address addr3(0x1000u, 0x12346u);
	assert(addr != addr3);
	assert(addr < addr3);


	Address addr4(0x1000u, 0x12345u);
	assert(addr == addr4);

	Address addr5(addr4.raw());
	assert(addr5 == addr4);

	Address addr6 = addr4.raw();
	assert(addr6 == addr4);
	
	uint64_t rawn=addr4.rawton();
	Address addr7(rawn, true);
	assert(addr7 == addr4);

	Address addr8(0x02u, 0x01u);
	uint64_t raw = 0x02ul<<32| 0x01;
	assert(addr8.raw() == raw);
    cout<<"addr8:"<<hex<<raw<<endl;

	return 0;
}
