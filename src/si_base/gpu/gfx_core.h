#pragma once

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_descriptor_allocator.h"
#include "si_base/gpu/gfx_resource_states_pool.h"
#include "si_base/gpu/gfx_descriptor_heap_pool.h"
#include "si_base/gpu/gfx_linear_allocator_page.h"

namespace SI
{
	struct GfxCoreDesc
	{
		uint32_t m_maxDescriptorCount = 1024;
		uint32_t m_maxResourceStates  = 512;
		uint32_t m_queueBufferCount   = 3;
		uint32_t m_maxViewDescriptorHeapCount    = 1024;
		uint32_t m_maxSamplerDescriptorHeapCount = 512;
	};

	class GfxCore : public Singleton<GfxCore>
	{
	public:
		GfxCore();
		~GfxCore();

		void Initialize(const GfxCoreDesc& desc);
		void Terminate();

		void Flip();

		GfxDescriptorAllocator& GetDescriptorAllocator(GfxDescriptorHeapType type)
		{
			return m_descriptorAllocators[(uint32_t)type];
		}

		GfxResourceStatesPool& GetResourceStatesPool()
		{
			return m_resourceStatePool;
		}

		GfxDescriptorHeapPool& GetViewDescriptorHeapPool()
		{
			return m_viewDescriptorHeapPool;
		}

		GfxDescriptorHeapPool& GetSamplerDescriptorHeapPool()
		{
			return m_samplerDescriptorHeapPool;
		}

		GfxLinearAllocatorPageManager& GetCpuLinearAllocatorPageManager()
		{
			return m_cpuLinearAllocatorPageManager;
		}

		GfxLinearAllocatorPageManager& GetGpuLinearAllocatorPageManager()
		{
			return m_gpuLinearAllocatorPageManager;
		}

	private:
		GfxDescriptorAllocator          m_descriptorAllocators[(uint32_t)GfxDescriptorHeapType::kMax];
		GfxResourceStatesPool           m_resourceStatePool;
		GfxDescriptorHeapPool           m_viewDescriptorHeapPool;
		GfxDescriptorHeapPool           m_samplerDescriptorHeapPool;
		GfxLinearAllocatorPageManager   m_cpuLinearAllocatorPageManager;
		GfxLinearAllocatorPageManager   m_gpuLinearAllocatorPageManager;
		bool                            m_initialized;
	};

} // namespace SI

#define SI_DESCRIPTOR_ALLOCATOR(type)      (GfxCore::GetInstance()->GetDescriptorAllocator(type))
#define SI_RESOURCE_STATES_POOL()          (GfxCore::GetInstance()->GetResourceStatesPool())
#define SI_VIEW_DESCRIPTOR_HEAP_POOL()     (GfxCore::GetInstance()->GetViewDescriptorHeapPool())
#define SI_SAMPLER_DESCRIPTOR_HEAP_POOL()  (GfxCore::GetInstance()->GetSamplerDescriptorHeapPool())
#define SI_CPU_LA_PAGE_MANAGER()           (GfxCore::GetInstance()->GetCpuLinearAllocatorPageManager())
#define SI_GPU_LA_PAGE_MANAGER()           (GfxCore::GetInstance()->GetGpuLinearAllocatorPageManager())
