#pragma once

#include <cstdint>

namespace SI
{
	struct Bitwise
	{
		// 立っているビットをカウントする.
		static uint32_t BitCount32(uint32_t mask);
		
		// 立っているビットをカウントする.
		static uint32_t BitCount64(uint64_t mask);

		// 最上位ビット取得. 失敗時(mask=0)は-1が返る.
		static int MSB32(uint32_t mask);

		// 最上位ビット取得. 失敗時(mask=0)は-1が返る.
		static int MSB64(uint64_t mask);

		// 最下位ビット取得. 失敗時(mask=0)は-1が返る.
		static int LSB32(uint32_t mask);

		// 最下位ビット取得. 失敗時(mask=0)は-1が返る.
		static int LSB64(uint64_t mask);

	};

} // namespace SI
