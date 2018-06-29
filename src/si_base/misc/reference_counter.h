#pragma once

#include <cstdint>
#include "si_base/core/basic_macro.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/memory/pool_allocator.h"
#include "si_base/concurency/atomic.h"

namespace SI
{
	class ReferenceCounter
	{
	public:
		ReferenceCounter()
			: m_counter(nullptr)
		{
		}

		~ReferenceCounter()
		{
			SI_ASSERT(m_counter == nullptr);
		}

		ReferenceCounter(const ReferenceCounter& c)
			: m_counter(c.m_counter)
		{
			AquireRef();
		}

		ReferenceCounter& operator=(const ReferenceCounter& c)
		{
			SI_ASSERT(m_counter == nullptr);

			m_counter = c.m_counter;
			AquireRef();
		}

	public:
		int Create()
		{
			SI_ASSERT(m_counter == nullptr);
			m_counter = SI_DEVICE_OBJ_ALLOCATOR().New<AtomicInt32>(1);

			return 1;
		}

		int AquireRef()
		{
			if(m_counter)
			{
				int afterValue = ++(*m_counter);
				SI_UNUSED(afterValue);
				SI_ASSERT(1 < afterValue, "1以下にはならないはず");

				return afterValue;
			}

			return -1;
		}

		int ReleaseRef()
		{
			if(m_counter)
			{
				SI_ASSERT(0<(*m_counter));

				int afterValue = --(*m_counter);
				SI_ASSERT(0 <= afterValue, "マイナスにはならないはず");

				if(afterValue==0)
				{
					SI_DEVICE_OBJ_ALLOCATOR().Delete(m_counter);
				}

				m_counter = nullptr; // 管理外とする.
				return afterValue;
			}

			return -1;
		}

		bool IsValid() const
		{
			return (m_counter != nullptr);
		}

	private:
		AtomicInt32*  m_counter;
	};
} // namespace SI
