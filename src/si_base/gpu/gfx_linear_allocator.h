#pragma once
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_buffer.h"

namespace SI
{
	class GfxLinearAllocatorPage;
	class GfxLinearAllocatorPageManager;

	class GfxLinearAllocatorMemory
	{
	public:
		GfxLinearAllocatorMemory(
			BaseBuffer* buffer,
			size_t     offset,
			size_t     size,
			void*      cpuAddr,
			GpuAddres  gpuAddr		
		)
			: m_buffer(buffer)
			, m_offset(offset)
			, m_size(size)
			, m_cpuAddr(cpuAddr)
			, m_gpuAddr(gpuAddr)
		{
		}
		
		GfxBuffer& GetBuffer()       { return m_buffer; }
		size_t     GetOffset()  const{ return m_offset; }
		size_t     GetSize()    const{ return m_size; }
		void*      GetCpuAddr() const{ return m_cpuAddr; }
		GpuAddres  GetGpuAddr() const{ return m_gpuAddr; }

	private:
		GfxBuffer  m_buffer;
		size_t     m_offset;
		size_t     m_size;
		void*      m_cpuAddr;
		GpuAddres  m_gpuAddr;
	};

	class GfxLinearAllocator
	{
	public:
		GfxLinearAllocator();
		~GfxLinearAllocator();

		void Initialize(bool cpuAccess);
		void Terminate();

		GfxLinearAllocatorMemory Allocate(size_t size, size_t alighnment=16);

		void Reset();

	private:
		void AllocateNewPage(size_t minimumPageSize);

	private:
		GfxLinearAllocatorPage*          m_currentPage;
		GfxLinearAllocatorPageManager*   m_pageManager;
		size_t                           m_offset;
	};

} // namespace SI
