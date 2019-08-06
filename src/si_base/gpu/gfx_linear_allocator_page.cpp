
#include "si_base/gpu/gfx_linear_allocator_page.h"

#include "si_base/core/new_delete.h"
#include "si_base/memory/pool_allocator.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_buffer.h"

namespace SI
{
	GfxLinearAllocatorPage::GfxLinearAllocatorPage()
		: m_pageSize(0)
		, m_buffer(nullptr)
		, m_cpuAddr(0)
		, m_gpuAddr(0)
	{
	}

	GfxLinearAllocatorPage::~GfxLinearAllocatorPage()
	{
		Terminate();
		SI_ASSERT(m_pageSize==0);
		SI_ASSERT(m_buffer==nullptr);
		SI_ASSERT(m_cpuAddr==0);
		SI_ASSERT(m_gpuAddr==0);
	}

	void GfxLinearAllocatorPage::Initialize(size_t pageSizeInByte, bool cpuAccess)
	{
		SI_ASSERT(m_pageSize == 0);
		m_pageSize = pageSizeInByte;

		BaseDevice& device = SI_BASE_DEVICE();

		SI_ASSERT(m_buffer == nullptr);
		GfxBufferDesc desc;
		desc.m_bufferSizeInByte = pageSizeInByte;
		if(cpuAccess)
		{
			desc.m_heapType       = GfxHeapType::Upload;
			desc.m_resourceFlags  = GfxResourceFlag::None;
			desc.m_resourceStates = GfxResourceState::GenericRead;
		}
		else
		{
			desc.m_heapType       = GfxHeapType::Default;
			desc.m_resourceFlags  = GfxResourceFlag::AllowUnorderedAccess;
			desc.m_resourceStates = GfxResourceState::UnorderedAccess;
		}
		m_buffer = device.CreateBuffer(desc);
				
		m_gpuAddr = m_buffer->GetGpuAddress();
		m_cpuAddr = (size_t)m_buffer->Map(0);
	}

	void GfxLinearAllocatorPage::Terminate()
	{
		if(m_buffer == nullptr) return;
		
		m_cpuAddr  = 0;
		m_gpuAddr  = 0;
		m_buffer->Unmap(0);

		BaseDevice& device = SI_BASE_DEVICE();
		device.ReleaseBuffer(m_buffer);
		m_buffer = nullptr;

		m_pageSize = 0;
	}

	/////////////////////////////////////////////////////////////////

	GfxLinearAllocatorPageManager::GfxLinearAllocatorPageManager()
		: m_queueBufferCount(0)
		, m_usingPages(nullptr)
		, m_currentPageIndex(0)
		, m_cpuAccess(true)
	{
	}

	GfxLinearAllocatorPageManager::~GfxLinearAllocatorPageManager()
	{
		Terminate();
		
		SI_ASSERT(m_queueBufferCount==0);
		SI_ASSERT(m_usingPages==nullptr);
	}
	
	void GfxLinearAllocatorPageManager::Initialize(uint32_t queueBufferCount, bool cpuAccess)
	{
		SI_ASSERT(m_queueBufferCount==0);
		m_queueBufferCount = queueBufferCount;
		
		SI_ASSERT(m_usingPages==nullptr);
		m_usingPages = SI_NEW_ARRAY(std::vector<GfxLinearAllocatorPage*>, m_queueBufferCount);

		m_avaiablePages.reserve(128);
		for(uint32_t i=0; i<m_queueBufferCount; ++i)
		{
			m_usingPages[i].reserve(128);
		}

		m_cpuAccess = cpuAccess;
	}
	
	void GfxLinearAllocatorPageManager::Terminate()
	{
		if(m_queueBufferCount==0) return; // initialized

		for(GfxLinearAllocatorPage* page : m_avaiablePages)
		{
			page->Terminate();
			SI_DEVICE_OBJ_ALLOCATOR().Delete(page);
		}
		m_avaiablePages.clear();
		
		for(uint32_t i=0; i<m_queueBufferCount; ++i)
		{
			auto& usingPages = m_usingPages[i];

			for(GfxLinearAllocatorPage* page : usingPages)
			{
				page->Terminate();
				SI_DEVICE_OBJ_ALLOCATOR().Delete(page);
			}
			usingPages.clear();
		}		
		SI_DELETE_ARRAY(m_usingPages);
		m_usingPages = nullptr;
		m_queueBufferCount = 0;
		m_currentPageIndex = 0;
		m_cpuAccess = true;
	}

	GfxLinearAllocatorPage* GfxLinearAllocatorPageManager::AllocateNewPage(size_t minimumPageSize)
	{
		MutexLocker locker(m_mutex);

		if(!m_avaiablePages.empty())
		{
			// 使えるpageがある場合、再利用する.
			GfxLinearAllocatorPage* page = m_avaiablePages.back();
			if(minimumPageSize <= page->GetPageSize())
			{
				m_avaiablePages.pop_back();

				m_usingPages[m_currentPageIndex].push_back(page);

				return page;
			}

			auto itr = m_avaiablePages.rbegin();
			++itr; // すでにbackは調べてる.
			
			for(; itr != m_avaiablePages.rend(); ++itr)
			{
				page = *itr;
				if(page->GetPageSize() < minimumPageSize) continue;

				m_avaiablePages.erase(itr.base());
				m_usingPages[m_currentPageIndex].push_back(page);

				return page;
			}
		}

		// 新しく作る.
		static const size_t kDefaultPageSize = 512 * 1024;
		SI_WARNING(minimumPageSize < kDefaultPageSize, "DefaultPageSizeを変えるか、LargePageを分けるほうが良さそう.");
		size_t pageSize = Max(minimumPageSize, kDefaultPageSize);
		GfxLinearAllocatorPage* newPage = CreateNewPage(pageSize);		
		m_usingPages[m_currentPageIndex].push_back(newPage);

		return newPage;
	}
	
	void GfxLinearAllocatorPageManager::Flip()
	{
		m_currentPageIndex = (m_currentPageIndex+1) % m_queueBufferCount;

		auto& usingPages = m_usingPages[m_currentPageIndex];

		if(!usingPages.empty())
		{
			// copy. m_avaiablePages = m_avaiablePages + usingPages
			m_avaiablePages.reserve(m_avaiablePages.size() + usingPages.size());
#if _MSC_VER >=1910 // VC2017
			std::copy(usingPages.begin(), usingPages.end(), std::back_inserter(m_avaiablePages));
#else
			usingPages.insert(usingPages.end(), m_avaiablePages.begin(), m_avaiablePages.end());
#endif
			usingPages.clear();
		}
	}
	
	GfxLinearAllocatorPage* GfxLinearAllocatorPageManager::CreateNewPage(size_t pageSize)
	{
		GfxLinearAllocatorPage* page = SI_DEVICE_OBJ_ALLOCATOR().New<GfxLinearAllocatorPage>();

		page->Initialize(pageSize, m_cpuAccess);

		return page;
	}
	
} // namespace SI
