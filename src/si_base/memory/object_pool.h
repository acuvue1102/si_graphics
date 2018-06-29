#pragma once

#include "si_base/memory/handle_allocator.h"
#include "si_base/core/new_delete.h"

namespace SI
{
	template<typename T>
	class ObjectPool
	{
	public:
		ObjectPool()
			: m_objects(nullptr)
			, m_objectCount(0)
			, m_allocatedObjectCount(0)
		{
		}

		~ObjectPool()
		{
			Terminate();
		}

		void Initialize(uint32_t maxObjectCount)
		{
			SI_ASSERT(m_objects == nullptr);
			SI_ASSERT(maxObjectCount != 0);
			
			m_objectCount = maxObjectCount;
			m_objects = SI_NEW_ARRAY(T, m_objectCount);
			m_handleAllocator.Initialize(m_objectCount);
			m_allocatedObjectCount = 0;
		}

		void Terminate()
		{
			if(m_objects==nullptr) return;
			SI_ASSERT(m_allocatedObjectCount==0, "開放忘れ");

			m_handleAllocator.Terminate();
			SI_DELETE_ARRAY(m_objects);
			m_objectCount = 0;
			m_allocatedObjectCount = 0;
		}

		T* Allocate()
		{
			if(m_objectCount <= m_allocatedObjectCount) return nullptr;
			SI_ASSERT(m_objects);

			uint32_t handle = m_handleAllocator.Allocate();
			T* obj = &m_objects[handle];
			SI_ASSERT(obj != nullptr);

			++m_allocatedObjectCount;
			return obj;
		}

		void Deallocate(T* obj)
		{
			if(obj == nullptr) return;
			SI_ASSERT(m_objects);
			SI_ASSERT(0<m_allocatedObjectCount);

			uintptr_t diff = ((uintptr_t)obj) - ((uintptr_t)m_objects);
			SI_ASSERT((diff % sizeof(T)) == 0);
			uint32_t handle = (diff % sizeof(T));
			m_handleAllocator.Deallocate(handle);

			--m_allocatedObjectCount;
		}

		T* GetObjectArray()
		{
			return m_objects;
		}

		uint32_t GetObjectCount() const
		{
			return m_objectCount;
		}

	private:
		T*              m_objects;
		uint32_t        m_objectCount;
		uint32_t        m_allocatedObjectCount;
		HandleAllocator m_handleAllocator;
	};

} // namespace SI


