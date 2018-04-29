#pragma once

namespace SI
{
	template<typename T, size_t SIZE>
	constexpr size_t ArraySize(const T (&)[SIZE])
	{
		return SIZE;
	}

	template<typename T>
	const T& Min(const T& a, const T& b)
	{
		return (a<=b)? a : b;
	}

	template<typename T>
	const T& Max(const T& a, const T& b)
	{
		return (a<b)? b : a;
	}
}
