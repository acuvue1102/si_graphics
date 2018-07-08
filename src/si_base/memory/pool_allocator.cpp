
#include "si_base/memory/pool_allocator.h"

#include <cstdint>
#include "si_base/memory/dlmalloc.h"
#include "si_base/core/core.h"

namespace SI
{
	PoolAllocator::PoolAllocator()
		: m_poolMemory(nullptr)
		, m_poolMemorySize(0)
		, m_mspace(nullptr)
	{
	}

	PoolAllocator::~PoolAllocator()
	{
		SI_ASSERT(m_mspace  == nullptr);
		SI_ASSERT(m_poolMemory == nullptr);
		SI_ASSERT(m_poolMemorySize == 0);
	}
	
	void PoolAllocator::Initialize(void* poolMemory, size_t poolMemorySize)
	{
		m_poolMemory = poolMemory;
		m_poolMemorySize = poolMemorySize;

		SI_ASSERT(m_mspace == nullptr);
		m_mspace = create_mspace_with_base(poolMemory , poolMemorySize , 1); // thread safeのためlockedにしておく.
	}
	
	void PoolAllocator::Terminate()
	{
		if(m_mspace == nullptr) return;

		destroy_mspace(m_mspace);
		m_mspace = 0;
		m_poolMemory = nullptr;
		m_poolMemorySize = 0;
	}

	void* PoolAllocator::Allocate(size_t size)
	{
		void* p = mspace_malloc(m_mspace, size);
		SI_ASSERT(p != nullptr);
		return p;
	}

	void* PoolAllocator::Allocate(size_t size, size_t alignment)
	{
		void* p = mspace_memalign(m_mspace, alignment, size);
		SI_ASSERT(p != nullptr);
		return p;
	}

	void PoolAllocator::Deallocate(void* p)
	{
		SI_ASSERT((uintptr_t)m_poolMemory <= (uintptr_t)p);
		SI_ASSERT((uintptr_t)p<(uintptr_t)m_poolMemory + m_poolMemorySize);
		mspace_free(m_mspace, p);
	}

	//////////////////////////////////////////////////////////////////
	
	PoolAllocatorEx::PoolAllocatorEx()
	{
	}
	
	PoolAllocatorEx::~PoolAllocatorEx()
	{
	}
	
	void PoolAllocatorEx::InitializeEx(size_t poolMemorySize)
	{
		void* poolMemory = (void*)SI_NEW_ARRAY(uint8_t, poolMemorySize);
		PoolAllocator::Initialize(poolMemory, poolMemorySize);
	}
	
	void PoolAllocatorEx::TerminateEx()
	{
		uint8_t* poolMemory =  (uint8_t*)m_poolMemory;
		PoolAllocator::Terminate();
		
		SI_DELETE_ARRAY(m_poolMemory);
	}

} // namespace SI
