
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"
#include "si_base/gpu/gfx_descriptor_heap.h"

namespace SI
{
	GfxCpuDescriptor GfxDescriptorHeap::GetCpuDescriptor(uint32_t descriptorIndex) const
	{
		return m_base->GetCpuDescriptor(descriptorIndex);
	}

	GfxGpuDescriptor GfxDescriptorHeap::GetGpuDescriptor(uint32_t descriptorIndex) const
	{
		return m_base->GetGpuDescriptor(descriptorIndex);
	}
} // namespace SI
