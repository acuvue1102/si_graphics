#pragma once

#include "si_base/core/assert.h"
#include "si_base/core/non_copyable.h"

namespace SI
{
	template <typename T>
	class Singleton : private NonCopyable
	{
	public:
		static T* GetInstance()
		{
			SI_ASSERT(s_instance!=nullptr);
			return s_instance;
		}

	protected:
		Singleton(T* ptr)
		{
			SI_ASSERT(s_instance==nullptr);
			s_instance = ptr;
		}

		~Singleton()
		{
			SI_ASSERT(s_instance!=nullptr);
			s_instance = nullptr;
		}

	private:
		Singleton() = delete;
		static T* s_instance;
	};

	template<typename T> T* Singleton<T>::s_instance = nullptr;
}
