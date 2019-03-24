
#include "si_base/gpu/gfx_descriptor_heap_ex.h"

#include "si_base/gpu/gfx_device.h"

namespace SI
{
	GfxDescriptorHeapEx::GfxDescriptorHeapEx()
	{
	}

	GfxDescriptorHeapEx::~GfxDescriptorHeapEx()
	{
		Terminate();
		SI_ASSERT(!m_descriptorHeap.IsValid());
		SI_ASSERT(!m_ref.IsValid());
	}

	void GfxDescriptorHeapEx::Initialize(const GfxDescriptorHeapDesc& desc)
	{
		Terminate();

		GfxDevice& device = *GfxDevice::GetInstance();
		
		SI_ASSERT(!m_descriptorHeap.IsValid());
		m_descriptorHeap = device.CreateDescriptorHeap(desc);
		m_ref.Create();
	}

	void GfxDescriptorHeapEx::InitializeAsCbvSrvUav(
		uint32_t srvCount,
		GfxDescriptorHeapFlag flag)
	{
		GfxDescriptorHeapDesc desc;
		desc.m_descriptorCount = srvCount;
		desc.m_type = GfxDescriptorHeapType::CbvSrvUav;
		desc.m_flag = flag;
		Initialize(desc);
	}

	void GfxDescriptorHeapEx::InitializeAsSampler(
		uint32_t samplerCount,
		GfxDescriptorHeapFlag flag)
	{
		GfxDescriptorHeapDesc desc;
		desc.m_descriptorCount = samplerCount;
		desc.m_type = GfxDescriptorHeapType::Sampler;
		desc.m_flag = flag;
		Initialize(desc);
	}

	void GfxDescriptorHeapEx::Terminate()
	{
		if(!m_descriptorHeap.IsValid()) return;
				
		if(m_ref.ReleaseRef()==0)
		{
			GfxDevice& device = *GfxDevice::GetInstance();
			device.ReleaseDescriptorHeap(m_descriptorHeap);
		}

		m_descriptorHeap = GfxDescriptorHeap();
	}
	
	void GfxDescriptorHeapEx::SetShaderResourceView(
		uint32_t index,
		GfxTexture& texture,
		const GfxShaderResourceViewDesc& srvDesc)
	{
		GfxDevice& device = *GfxDevice::GetInstance();
		device.CreateShaderResourceView(m_descriptorHeap, index, texture, srvDesc);
	}
	
	void GfxDescriptorHeapEx::SetSampler(uint32_t index, const GfxSamplerDesc& samplerDesc)
	{
		GfxDevice& device = *GfxDevice::GetInstance();
		device.CreateSampler(m_descriptorHeap, index, samplerDesc);
	}

} // namespace SI
