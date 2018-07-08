
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_core.h"

namespace SI
{
	GfxCore::GfxCore()
		: Singleton<GfxCore>(this)
		, m_initialized(false)
	{
	}

	GfxCore::~GfxCore()
	{
		Terminate();
	}
	
	void GfxCore::Initialize(const GfxCoreDesc& desc)
	{
		SI_ASSERT(!m_initialized);

		for(uint32_t i=0; i<ArraySize(m_descriptorAllocators); ++i)
		{
			GfxDescriptorAllocator& alloc = m_descriptorAllocators[i];
			GfxDescriptorHeapType type = (GfxDescriptorHeapType)i;

			alloc.Initialize(type, desc.m_maxDescriptorCount);
		}

		m_resourceStatePool.Initialize(desc.m_maxResourceStates);
		
		m_viewDescriptorHeapPool.Initialize(GfxDescriptorHeapType::CbvSrvUav, desc.m_maxViewDescriptorHeapCount);
		m_samplerDescriptorHeapPool.Initialize(GfxDescriptorHeapType::Sampler, desc.m_maxSamplerDescriptorHeapCount);
		
		m_cpuLinearAllocatorPageManager.Initialize(desc.m_queueBufferCount, true);
		m_gpuLinearAllocatorPageManager.Initialize(desc.m_queueBufferCount, false);

		m_initialized = true;
	}

	void GfxCore::Terminate()
	{
		if(!m_initialized) return;
				
		m_gpuLinearAllocatorPageManager.Terminate();
		m_cpuLinearAllocatorPageManager.Terminate();

		m_samplerDescriptorHeapPool.Terminate();
		m_viewDescriptorHeapPool.Terminate();

		m_resourceStatePool.Terminate();

		for(uint32_t i=0; i<ArraySize(m_descriptorAllocators); ++i)
		{
			GfxDescriptorAllocator& alloc = m_descriptorAllocators[i];
			alloc.Terminate();
		}

		m_initialized = false;
	}
	
	void GfxCore::Flip()
	{
		m_viewDescriptorHeapPool.Flip();
		m_samplerDescriptorHeapPool.Flip();
	}
	
} // namespace SI
