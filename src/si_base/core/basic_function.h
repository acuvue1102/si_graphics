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

	template <typename T>
	T AlignUp(T value, size_t alignment)
	{
		return ((value + (T)(alignment-1)) & ~((T)(alignment-1)));
	}

	template <typename T> 
	T AlignDown(T value, size_t alignment)
	{
		return (value & ~((T)(alignment-1)));
	}

	template <typename T>
	bool IsPowerOfTwo(T value)
	{
		return ((value & (value-1)) == 0);
	}
}
