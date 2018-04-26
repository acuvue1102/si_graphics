#pragma once

#include "base/safe_delete.h"
#include "base/scope_exit.h"
#include "base/assert.h"

//#define SI_ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

namespace SI
{
	template<typename T, size_t SIZE>
	constexpr size_t ArrayCount(const T (&)[SIZE])
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
