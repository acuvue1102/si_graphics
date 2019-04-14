
#include "si_base/memory/linear_allocator.h"

#include "si_base/core/new_delete.h"

namespace SI
{
	class LinearAllocatorPage
	{
	public:
		LinearAllocatorPage()
			: m_addr(nullptr)
			, m_size(0)
		{}

		LinearAllocatorPage(size_t size)
			: m_addr(malloc(size))
			, m_size(size)
		{
			m_endAddr = (uintptr_t)m_addr + m_size;
		}

		~LinearAllocatorPage()
		{
			if(m_addr)
			{
				free(m_addr);
			}
		}
		
		void*      GetAddress()    const{ return m_addr; }
		size_t     GetSize()       const{ return m_size; }
		uintptr_t  GetEndAddr()    const{ return m_endAddr; }

	private:
		void*       m_addr;
		size_t      m_size;
		uintptr_t   m_endAddr;
	};

	////////////////////////////////////////////////////////////////

	LinearAllocator::LinearAllocator()
		: m_currentPage(nullptr)
		, m_currentAddr(0)
		, m_defaultPageSize(65536)
	{
	}
	
	LinearAllocator::~LinearAllocator()
	{
		Terminate();
	}
	
	void LinearAllocator::Initialize(size_t defaultPageSize)
	{
		m_defaultPageSize = defaultPageSize;
	}
	
	void LinearAllocator::Terminate()
	{
		for(LinearAllocatorPage* page : m_allPages)
		{
			SI_DELETE(page);
		}
		m_allPages.clear();
		m_unusedPages.clear();

		for(LinearAllocatorPage* page : m_allLargePages)
		{
			SI_DELETE(page);
		}
		m_allLargePages.clear();
		m_unusedLargePages.clear();

		m_currentPage = nullptr;
		m_currentAddr = 0;
		m_defaultPageSize = 65536;
	}
	
	void* LinearAllocator::Allocate(size_t size)
	{
		return Allocate(size, 16);
	}

	void* LinearAllocator::Allocate(size_t size, size_t alignment)
	{
		SI_ASSERT(IsPowerOfTwo(alignment));
		m_currentAddr = AlignUp(m_currentAddr, alignment);

		if(m_currentPage == nullptr)
		{
			AllocateNewPage(size+alignment);
			m_currentAddr = AlignUp(m_currentAddr, alignment);
		}
		else if(m_currentPage->GetEndAddr() < (m_currentAddr + size))
		{
			AllocateNewPage(size+alignment);
			m_currentAddr = AlignUp(m_currentAddr, alignment);
		}

		void* ret = (void*)m_currentAddr;
		m_currentAddr += size;

		return ret;
	}
	
	void LinearAllocator::Reset()
	{
		m_unusedPages = m_allPages;
		m_unusedLargePages = m_allLargePages;

		m_currentPage = nullptr;
		m_currentAddr = 0;
	}

	void LinearAllocator::AllocateNewPage(size_t minimumPageSize)
	{
		// 標準のPageSize以上かどうか.
		if(m_defaultPageSize < minimumPageSize)
		{
			for(auto itr = m_unusedLargePages.begin(); itr!=m_unusedLargePages.end(); ++itr)
			{
				LinearAllocatorPage* page = (*itr);
				if(minimumPageSize < page->GetSize()) continue;

				// 必要なサイズ以上のPageを見つけたので再利用.
				m_unusedLargePages.erase(itr);
				m_currentPage = page;
				m_currentAddr = (uintptr_t)m_currentPage->GetAddress();
				return;
			}
			
			// 新しく作る.
			LinearAllocatorPage* newPage = SI_NEW(LinearAllocatorPage, minimumPageSize);
			m_allLargePages.push_back(newPage);
			m_currentPage = newPage;
			m_currentAddr = (uintptr_t)m_currentPage->GetAddress();
			return;
		}
		else
		{
			if(!m_unusedPages.empty())
			{
				// 再利用.
				LinearAllocatorPage* page = m_unusedPages.back();
				m_unusedPages.pop_back();

				m_currentPage = page;
				m_currentAddr = (uintptr_t)m_currentPage->GetAddress();
				return;
			}

			// 新しく作る.
			LinearAllocatorPage* newPage = SI_NEW(LinearAllocatorPage, m_defaultPageSize);
			m_allPages.push_back(newPage);
			m_currentPage = newPage;
			m_currentAddr = (uintptr_t)m_currentPage->GetAddress();
		}
	}

} // namespace SI
