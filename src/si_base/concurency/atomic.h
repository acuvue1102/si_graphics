#pragma once

#include <cstdint>
#include <atomic>

namespace SI
{
	// 後でOSのAPIによる実装に置き替えたくなるかもしれないから
	// 念のため、特別なクラスを用意する.
	template <typename T>
	class Atomic
	{
	public:
		Atomic()
			: m_value(0)
		{
		}

		Atomic(T value)
			: m_value(value)
		{
		}
		
		T operator =(T value) volatile{ return m_value.operator=(value); }
		T operator =(T value)         { return m_value.operator=(value); }
		
		T operator+=(T value) volatile{ return m_value.operator+=(value); }
		T operator+=(T value)         { return m_value.operator+=(value); }
		T operator-=(T value) volatile{ return m_value.operator-=(value); }
		T operator-=(T value)         { return m_value.operator-=(value); }
				
		T operator&=(T value) volatile{ return m_value.operator&=(value); }
		T operator&=(T value)         { return m_value.operator&=(value); }
		T operator|=(T value) volatile{ return m_value.operator|=(value); }
		T operator|=(T value)         { return m_value.operator|=(value); }
		T operator^=(T value) volatile{ return m_value.operator^=(value); }
		T operator^=(T value)         { return m_value.operator^=(value); }
		
		// 前置
		T operator++()        volatile{ return m_value.operator++(); }
		T operator++()                { return m_value.operator++(); }
		T operator--()        volatile{ return m_value.operator--(); }
		T operator--()                { return m_value.operator--(); }

		// 後置
		T operator++(T dummy) volatile{ return m_value.operator++(dummy); }
		T operator++(T dummy)         { return m_value.operator++(dummy); }
		T operator--(T dummy) volatile{ return m_value.operator--(dummy); }
		T operator--(T dummy)         { return m_value.operator--(dummy); }

		operator T()             const{ return m_value; }

	private:
		std::atomic<T> m_value;
	};
	
	using AtomicUint32 = Atomic<uint32_t>;
	using AtomicInt32  = Atomic<int32_t>;
	using AtomicUint64 = Atomic<uint32_t>;
	using AtomicInt64  = Atomic<int32_t>;

} // namespace SI

