#pragma once

#include <cstdint>
#include "si_base/core/assert.h"

namespace SI
{
	template<typename T>
	class Array
	{
	public:
		Array()
			: m_items(nullptr)
			, m_itemCount(0u)
		{
		}

		Array(T* items, uint32_t itemCount)
			: m_items(items)
			, m_itemCount(itemCount)
		{
		}

		Array(const Array<T>& a)
			: m_items(a.m_items)
			, m_itemCount(a.m_itemCount)
		{
		}

		~Array()
		{
			SI_ASSERT(m_items==nullptr);
			SI_ASSERT(m_itemCount==0);
		}

		void Reset()
		{
			m_items     = nullptr;
			m_itemCount = 0u;
		}

		void Setup(T* items, uint32_t itemCount)
		{
			m_items     = items;
			m_itemCount = itemCount;
		}

		void SetItem(uint32_t index, const T& item)
		{
			m_items[index] = item;
		}
		
		const T& GetItem(int index) const
		{
			return m_items[index];
		}
		
		T& GetItem(int index)
		{
			return m_items[index];
		}

		T* GetItemsAddr()
		{
			return m_items;
		}

		const T* GetItemsAddr() const
		{
			return m_items;
		}

		uint32_t GetItemCount() const
		{
			return m_itemCount;
		}

		bool IsValid() const
		{
			return m_items != nullptr;
		}
		
	public:
		const T& operator[](int index) const
		{
			return m_items[index];
		}
		
		T& operator[](int index)
		{
			return m_items[index];
		}
		
		Array<T>& operator=(const Array<T>& a)
		{
			if(this == &a) return (*this);

			m_items     = a.m_items;
			m_itemCount = a.m_itemCount;
			return (*this);
		}

	private:
		T*        m_items;
		uint32_t  m_itemCount;
	};


	//////////////////////////////////////////////////////////////////////////

	
	template<typename T>
	class ConstArray
	{
	public:
		ConstArray()
			: m_items(nullptr)
			, m_itemCount(0u)
		{
		}

		ConstArray(const T* items, uint32_t itemCount)
			: m_items(items)
			, m_itemCount(itemCount)
		{
		}

		ConstArray(const ConstArray<T>& a)
			: m_items(a.m_items)
			, m_itemCount(a.m_itemCount)
		{
		}

		ConstArray(const Array<T>& a)
			: m_items(a.GetItemsAddr())
			, m_itemCount(a.GetItemCount())
		{
		}

		~ConstArray()
		{
			SI_ASSERT(m_items==nullptr);
			SI_ASSERT(m_itemCount==0);
		}

		void Reset()
		{
			m_items     = nullptr;
			m_itemCount = 0u;
		}

		void Setup(const T* items, uint32_t itemCount)
		{
			m_items     = items;
			m_itemCount = itemCount;
		}
		
		const T& GetItem(int index) const
		{
			return m_items[index];
		}

		const T* GetItemsAddr() const
		{
			return m_items;
		}

		uint32_t GetItemCount() const
		{
			return m_itemCount;
		}
		
	public:
		const T& operator[](int index) const
		{
			return m_items[index];
		}
		
		ConstArray<T>& operator=(const ConstArray<T>& a)
		{
			if(this == &a) return (*this);

			m_items     = a.m_items;
			m_itemCount = a.m_itemCount;
			return (*this);
		}
		
		ConstArray<T>& operator=(const Array<T>& a)
		{
			m_items     = a.GetItemsAddr();
			m_itemCount = a.GetItemCount();
			return (*this);
		}

	private:
		const T*  m_items;
		uint32_t  m_itemCount;
	};



} // namespace SI
