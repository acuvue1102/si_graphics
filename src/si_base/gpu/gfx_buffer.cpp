
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/gfx_buffer.h"

namespace SI
{
	void* GfxBuffer::Map(uint32_t subResourceId)
	{
		return m_base->Map(subResourceId);
	}

	void  GfxBuffer::Unmap(uint32_t subResourceId)
	{
		m_base->Unmap(subResourceId);
	}
	
	size_t GfxBuffer::GetSize() const
	{
		return m_base->GetSize();
	}

	GpuAddress GfxBuffer::GetGpuAddress() const
	{
		return m_base->GetGpuAddress();
	}
	
} // namespace SI
