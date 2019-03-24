
#include "si_base/misc/bitwise.h"
#include "si_base/core/core.h"

#if _WIN32
#include <intrin.h>
#endif

namespace SI
{
#if _WIN32
	// 立っているビットをカウントする.
	uint32_t Bitwise::BitCount32(uint32_t mask)
	{
		return __popcnt(mask);
	}

	// 立っているビットをカウントする.
	uint32_t Bitwise::BitCount64(uint64_t mask)
	{
		return (uint32_t)__popcnt64(mask);
	}

	int Bitwise::MSB32(uint32_t mask)
	{
		if(mask == 0) return -1;

		unsigned long index = 0xffffffff;
		uint8_t ret = _BitScanReverse(&index, mask);
		SI_UNUSED(ret);
		SI_ASSERT(ret!=0 && index != 0xffffffff);

		return (int)index;
	}

	int Bitwise::MSB64(uint64_t mask)
	{
		if(mask == 0) return -1;

		unsigned long index = 0xffffffff;
		uint8_t ret = _BitScanReverse64(&index, mask);
		SI_UNUSED(ret);
		SI_ASSERT(ret!=0 && index != 0xffffffff);

		return (int)index;
	}
	
	int Bitwise::LSB32(uint32_t mask)
	{
		if(mask == 0) return -1;

		unsigned long index = 0xffffffff;
		uint8_t ret = _BitScanForward(&index, mask);
		SI_UNUSED(ret);
		SI_ASSERT(ret!=0 && index != 0xffffffff);

		return (int)index;
	}

	int Bitwise::LSB64(uint64_t mask)
	{
		if(mask == 0) return -1;

		unsigned long index = 0xffffffff;
		uint8_t ret = _BitScanForward64(&index, mask);
		SI_UNUSED(ret);
		SI_ASSERT(ret!=0 && index != 0xffffffff);

		return (int)index;
	}
#else
	// 立っているビットをカウントする.
	int Bitwise::BitCount32(uint32_t mask)
	{
		mask = (mask & 0x55555555) + ((mask >>  1) & 0x55555555);
		mask = (mask & 0x33333333) + ((mask >>  2) & 0x33333333);
		mask = (mask & 0x0f0f0f0f) + ((mask >>  4) & 0x0f0f0f0f);
		mask = (mask & 0x00ff00ff) + ((mask >>  8) & 0x00ff00ff);
		mask = (mask & 0x0000ffff) + ((mask >> 16) & 0x0000ffff);

		return (int)(mask);
	}
		
	// 立っているビットをカウントする.
	int Bitwise::BitCount64(uint64_t mask)
	{
		mask = (mask & 0x5555555555555555) + ((mask >>  1) & 0x5555555555555555);
		mask = (mask & 0x3333333333333333) + ((mask >>  2) & 0x3333333333333333);
		mask = (mask & 0x0f0f0f0f0f0f0f0f) + ((mask >>  4) & 0x0f0f0f0f0f0f0f0f);
		mask = (mask & 0x00ff00ff00ff00ff) + ((mask >>  8) & 0x00ff00ff00ff00ff);
		mask = (mask & 0x0000ffff0000ffff) + ((mask >> 16) & 0x0000ffff0000ffff);
		mask = (mask & 0x00000000ffffffff) + ((mask >> 32) & 0x00000000ffffffff);

		return (int)(mask);
	}

	int Bitwise::MSB32(uint32_t mask)
	{
		if(mask == 0) return -1;

		mask |= (mask >> 1);
		mask |= (mask >> 2);
		mask |= (mask >> 4);
		mask |= (mask >> 8);
		mask |= (mask >> 16);
		return BitCount32(mask) - 1;
	}

	int Bitwise::MSB64(uint64_t mask)
	{
		if(mask == 0) return -1;

		mask |= (mask >> 1);
		mask |= (mask >> 2);
		mask |= (mask >> 4);
		mask |= (mask >> 8);
		mask |= (mask >> 16);
		mask |= (mask >> 32);
		return BitCount64(mask) - 1;
	}
	
	int Bitwise::LSB32(uint32_t mask)
	{
		if(mask == 0) return -1;

		mask |= (mask << 1);
		mask |= (mask << 2);
		mask |= (mask << 4);
		mask |= (mask << 8);
		mask |= (mask << 16);
		return 32 - BitCount32(mask);
	}

	int Bitwise::LSB64(uint64_t mask)
	{
		if(mask == 0) return -1;

		mask |= (mask << 1);
		mask |= (mask << 2);
		mask |= (mask << 4);
		mask |= (mask << 8);
		mask |= (mask << 16);
		mask |= (mask << 32);
		return 64 - BitCount64(mask);
	}
#endif
} // namespace SI
