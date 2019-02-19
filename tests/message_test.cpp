//message_test.cpp 

#include "../src/sn_message.h"
#include <string.h>

using namespace std;
using namespace snode;

int main()
{
	Address src(0x0001ul, 0x1234ul);
	Address dst(0x0001ul, 0x2345ul);
	const char* m="hello,world!";
	Message msg(src, dst, m, strlen(m));

	uint8_t* stream;
	size_t size;
	std::tie(stream, size) = msg.encode();

	assert(strlen(m)+24 == msg.size());
	assert(size == msg.size());
	assert(strlen(m) == msg.payload_size());
	assert( strcmp((char*)msg.payload(), m)==0 );


	Message msg2;
	msg2.decode(stream, size);

	assert(msg.dst() == msg2.dst());
	assert(msg.src() == msg2.src());
	assert(msg.payload_size() == msg2.payload_size());
	assert( strcmp((char*)msg.payload(), (char*)msg2.payload())==0 );
	
	return 0;
}
