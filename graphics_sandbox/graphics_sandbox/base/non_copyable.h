﻿#pragma once

#include "base/assert.h"

namespace SI
{
	class NonCopyable
	{
	public:
		NonCopyable()
		{
		}

		~NonCopyable()
		{
		}

	private:
		NonCopyable(const NonCopyable&)            = delete;
		NonCopyable(NonCopyable &&)                = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;
		NonCopyable& operator=(NonCopyable &&)     = delete;
	};
}
