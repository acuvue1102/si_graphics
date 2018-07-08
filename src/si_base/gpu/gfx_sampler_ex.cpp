
#include "si_base/gpu/gfx_sampler_ex.h"

#include "si_base/gpu/gfx_core.h"
#include "si_base/gpu/dx12/dx12_device.h"

namespace SI
{
	GfxSamplerEx::GfxSamplerEx()
	{
	}

	GfxSamplerEx::~GfxSamplerEx()
	{
		SI_ASSERT(m_descriptor.GetCpuDescriptor().m_ptr == 0);
	}

	void GfxSamplerEx::Initialize(const GfxSamplerDesc& samplerDesc)
	{
		SI_ASSERT(m_descriptor.GetCpuDescriptor().m_ptr == 0);

		BaseDevice& device = SI_BASE_DEVICE();
		m_descriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kSampler).Allocate(1);

		device.CreateSampler(m_descriptor, samplerDesc);
	}
	
	void GfxSamplerEx::Terminate()
	{
		if(m_descriptor.GetCpuDescriptor().m_ptr == 0) return;

		SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kSampler).Deallocate(m_descriptor);
		m_descriptor = GfxDescriptor();
	}

} // namespace SI
