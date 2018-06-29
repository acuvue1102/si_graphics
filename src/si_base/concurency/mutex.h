#pragma once

#include <cstdint>
#include <mutex>
#include "si_base/core/non_copyable.h"

namespace SI
{
	// 後でOSのAPIによる実装に置き替えたくなるかもしれないから
	// 念のため、特別なクラスを用意する.
	class Mutex : private NonCopyable
	{
	public:
		Mutex()
		{
		}

		~Mutex()
		{
		}

		void Lock()
		{
			m_mutex.lock();
		}

		void Unlock()
		{
			m_mutex.unlock();
		}

		bool TryLock()
		{
			return m_mutex.try_lock();
		}
		
	private:
		std::mutex m_mutex;
	};

	class MutexLocker : private NonCopyable
	{
	public:
		MutexLocker(Mutex& mutex)
			: m_mutex(&mutex)
		{
			m_mutex->Lock();
		}
		
		~MutexLocker()
		{
			m_mutex->Unlock();
		}

	private:		
		MutexLocker(){}

		Mutex* m_mutex;
	};

} // namespace SI

