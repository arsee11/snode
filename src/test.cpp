#include <cstdint>

	struct A{
		uint64_t en:40;
		uint64_t sn:24;
	}addr;

int main()
{
	uint64_t a=0x1100002200000000;
	addr = *(A*)&a;

	uint64_t b = *(uint64_t*)&addr;

	return 0;
}
