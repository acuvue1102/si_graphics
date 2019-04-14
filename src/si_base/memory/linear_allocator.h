#pragma once
#include <vector>
#include "si_base/memory/allocator_base.h"

namespace SI
{
	class LinearAllocatorPage;

	class LinearAllocator : public AllocatorBase
	{
	public:
		LinearAllocator();
		~LinearAllocator();

		void Initialize(size_t defaultPageSize = 65536);
		void Terminate();

		void* Allocate(size_t size) override;
		void* Allocate(size_t size, size_t alignment) override;

		void Deallocate(void* p) override{ SI_ASSERT("Not supported. Use Reset()."); }

		void Reset();

	private:
		void AllocateNewPage(size_t minimumPageSize);

	private:
		std::vector<LinearAllocatorPage*>  m_allPages;
		std::vector<LinearAllocatorPage*>  m_unusedPages;
		
		std::vector<LinearAllocatorPage*>  m_allLargePages;
		std::vector<LinearAllocatorPage*>  m_unusedLargePages;

		LinearAllocatorPage*               m_currentPage;
		uintptr_t                          m_currentAddr;
		size_t                             m_defaultPageSize;
	};

} // namespace SI
