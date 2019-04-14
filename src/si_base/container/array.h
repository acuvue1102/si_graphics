#pragma once

#include <cstdint>
#include "si_base/core/assert.h"
#include "si_base/core/new_delete.h"
#include "si_base/serialization/reflection.h"

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
		
		const T& GetItem(uint32_t index) const
		{
			return m_items[index];
		}
		
		T& GetItem(uint32_t index)
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

		void CopyFromArray(const T* items, uint32_t itemCount)
		{
			for(uint32_t i=0; i<itemCount; ++i)
			{
				m_items[i] = items[i];
			}
		}

		void CopyToArray(T* items, uint32_t itemCount) const
		{
			for(uint32_t i=0; i<itemCount; ++i)
			{
				items[i] = m_items[i];
			}
		}

		bool IsValid() const
		{
			return m_items != nullptr;
		}
		
	public:
		const T& operator[](size_t index) const
		{
			return m_items[index];
		}
		
		T& operator[](size_t index)
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

		SI_TEMPLATE1_REFLECTION(SI::Array, T,
			SI_REFLECTION_MEMBER(m_items),
			SI_REFLECTION_MEMBER(m_itemCount))
	};

	//////////////////////////////////////////////////////////////////////////
	
	template<typename T>
	class SafeArray
	{
	public:
		SafeArray()
			: m_items(nullptr)
			, m_itemCount(0u)
		{
		}

		explicit SafeArray(uint32_t itemCount)
			: m_items(SI_NEW_ARRAY(T, itemCount))
			, m_itemCount(itemCount)
		{
		}

		SafeArray(const SafeArray<T>& a)
			: m_items(nullptr)
			, m_itemCount(0u)
		{
			if(a.IsValid() && a.GetItemCount()>0)
			{
				m_items = SI_NEW_ARRAY(T, a.GetItemCount());
				m_itemCount = a.GetItemCount();
				for(uint32_t i=0; i<m_itemCount; ++i)
				{
					m_items[i] = a.GetItem(i);
				}
			}
		}

		SafeArray(SafeArray<T>&& a) noexcept
			: m_items(a.m_items)
			, m_itemCount(a.m_itemCount)
		{
			a.m_items = nullptr;
			a.m_itemCount = 0;
		}

		~SafeArray()
		{
			SI_DELETE_ARRAY(m_items);
		}

		void Reset()
		{
			SI_DELETE_ARRAY(m_items);
			m_items     = nullptr;
			m_itemCount = 0u;
		}

		void Setup(uint32_t itemCount)
		{
			Reset();
			m_items = SI_NEW_ARRAY(T, itemCount);
			m_itemCount = itemCount;
		}

		void SetItem(uint32_t index, const T& item)
		{
			m_items[index] = item;
		}
		
		const T& GetItem(uint32_t index) const
		{
			return m_items[index];
		}
		
		T& GetItem(uint32_t index)
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

		void CopyFromArray(const T* items, uint32_t itemCount)
		{
			for(uint32_t i=0; i<itemCount; ++i)
			{
				m_items[i] = items[i];
			}
		}

		void CopyToArray(T* items, uint32_t itemCount) const
		{
			for(uint32_t i=0; i<itemCount; ++i)
			{
				items[i] = m_items[i];
			}
		}

		bool IsValid() const
		{
			return m_items != nullptr;
		}
		
	public:
		const T& operator[](size_t index) const
		{
			return m_items[index];
		}
		
		T& operator[](size_t index)
		{
			return m_items[index];
		}
		
		SafeArray<T>& operator=(const SafeArray<T>& a)
		{
			if(this == &a) return (*this);

			Reset();
			if(a.IsValid() && a.GetItemCount()>0)
			{
				m_items = SI_NEW_ARRAY(T, a.GetItemCount());
				m_itemCount = a.GetItemCount();
				for(uint32_t i=0; i<m_itemCount; ++i)
				{
					m_items[i] = a.GetItem(i);
				}
			}
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
		
		const T& GetItem(uint32_t index) const
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

		void CopyToArray(T* items, uint32_t itemCount) const
		{
			for(uint32_t i=0; i<itemCount; ++i)
			{
				items[i] = m_items[i];
			}
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
