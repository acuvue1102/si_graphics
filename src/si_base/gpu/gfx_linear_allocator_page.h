#pragma once

#include <vector>
#include "si_base/core/singleton.h"
#include "si_base/gpu/gfx_config.h"
#include "si_base/memory/handle_allocator.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/concurency/mutex.h"

namespace SI
{
	class BaseBuffer;

	class GfxLinearAllocatorPage
	{
	public:
		GfxLinearAllocatorPage();
		~GfxLinearAllocatorPage();

		void Initialize(size_t pageSizeInByte, bool cpuAccess);
		void Terminate();

		size_t      GetPageSize() const{ return m_pageSize; }
		size_t      GetCpuAddr()  const{ return m_cpuAddr; }
		GpuAddress   GetGpuAddr()  const{ return m_gpuAddr; }
		BaseBuffer* GetBaseBuffer(){ return m_buffer; }

	private:
		size_t      m_pageSize;
		BaseBuffer* m_buffer;
		size_t      m_cpuAddr;
		GpuAddress   m_gpuAddr;
	};
	
	class GfxLinearAllocatorPageManager
	{
	public:
		GfxLinearAllocatorPageManager();
		~GfxLinearAllocatorPageManager();

		void Initialize(uint32_t queueBufferCount, bool cpuAccess);
		void Terminate();

		GfxLinearAllocatorPage* AllocateNewPage(size_t minimumPageSize);

		void Flip();

	private:
		GfxLinearAllocatorPage* CreateNewPage(size_t pageSize);

	private:
		uint32_t                              m_queueBufferCount;
		std::vector<GfxLinearAllocatorPage*>  m_avaiablePages;
		std::vector<GfxLinearAllocatorPage*>* m_usingPages;
		uint32_t                              m_currentPageIndex;
		Mutex                                 m_mutex;
		bool                                  m_cpuAccess;
	};

} // namespace SI
