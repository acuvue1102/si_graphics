
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"
#include "si_base/gpu/gfx_descriptor_heap.h"

namespace SI
{
	GfxCpuDescriptor GfxDescriptorHeap::GetCpuDescriptor(GfxDescriptorHeapType type, uint32_t descriptorIndex) const
	{
		return m_base->GetCpuDescriptor(type, descriptorIndex);
	}

	GfxGpuDescriptor GfxDescriptorHeap::GetGpuDescriptor(GfxDescriptorHeapType type, uint32_t descriptorIndex) const
	{
		return m_base->GetGpuDescriptor(type, descriptorIndex);
	}
} // namespace SI
