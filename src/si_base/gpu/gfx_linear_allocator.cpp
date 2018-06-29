
#include "si_base/gpu/gfx_linear_allocator.h"

#include "si_base/core/basic_function.h"
#include "si_base/gpu/gfx_core.h"
#include "si_base/gpu/gfx_linear_allocator_page.h"

namespace SI
{
	GfxLinearAllocator::GfxLinearAllocator()
		: m_currentPage(nullptr)
		, m_pageManager(nullptr)
		, m_offset(0)
	{
	}
	
	GfxLinearAllocator::~GfxLinearAllocator()
	{
	}
	
	void GfxLinearAllocator::Initialize(bool cpuAccess)
	{
		m_pageManager = cpuAccess? &SI_CPU_LA_PAGE_MANAGER() : &SI_GPU_LA_PAGE_MANAGER();
		SI_ASSERT(m_pageManager);

		m_offset = 0;
	}
	
	void GfxLinearAllocator::Terminate()
	{
		m_pageManager = nullptr;
		m_currentPage = nullptr;
		m_offset = 0;
	}

	GfxLinearAllocatorMemory GfxLinearAllocator::Allocate(size_t size, size_t alignment)
	{
		if(m_currentPage == nullptr)
		{
			AllocateNewPage(size);
		}
		else if(m_currentPage->GetPageSize() < (m_offset + size))
		{
			AllocateNewPage(size);
		}

		SI_ASSERT(IsPowerOfTwo(alignment));
		m_offset = AlignUp(m_offset, alignment);

		size_t offset = m_offset;
		m_offset += size;

		return GfxLinearAllocatorMemory(
			m_currentPage->GetBaseBuffer(),
			offset,
			size,
			(void*)(m_currentPage->GetCpuAddr() + offset),
			m_currentPage->GetCpuAddr() + offset);
	}
	
	void GfxLinearAllocator::Reset()
	{
		m_currentPage = nullptr;
		m_offset      = 0;
	}

	void GfxLinearAllocator::AllocateNewPage(size_t minimumPageSize)
	{
		m_currentPage = m_pageManager->AllocateNewPage(minimumPageSize);
		m_offset      = 0;
	}

} // namespace SI
