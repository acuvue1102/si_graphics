#pragma once

// SCOPE_EXITマクロで使われるオブジェクト

#include <functional>
#include "si_base/core/basic_macro.h"

namespace SI
{
	class ScopeExit
	{
	public:
		ScopeExit(std::function<void(void)> f)
			: m_func(f)
		{
		}

		~ScopeExit()
		{
			m_func();
		}

	private:
		std::function<void(void)> m_func;
	};

} // namespace SI

#define SI_SCOPE_EXIT(...) SI::ScopeExit SI_JOIN(se, __LINE__)([&]{__VA_ARGS__})
