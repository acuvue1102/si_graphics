#pragma once

#include "si_base/gpu/gfx_enum.h"
#include "si_base/memory/handle_allocator.h"

namespace SI
{
	class GfxGpuResource;

	// 今のGpuResourceのStateを管理するためのPool
	class GfxResourceStatesPool
	{
	public:
		GfxResourceStatesPool();
		~GfxResourceStatesPool();

		void Initialize(uint32_t maxStateCount);
		void Terminate();

		void AllocateHandle(GfxGpuResource* gpuResource);
		void DeallocateHandle(GfxGpuResource* gpuResource);		
		
		void SetResourceStates(uint32_t resourceStateHanlde, GfxResourceStates states);
		void SetResourceStates(const GfxGpuResource* gpuResource, GfxResourceStates states);
		GfxResourceStates GetResourceStates(uint32_t resourceStateHanlde);
		GfxResourceStates GetResourceStates(const GfxGpuResource* gpuResource);
		GfxGpuResource*   GetGpuResource(uint32_t resourceStateHanlde);
		
		uint32_t GetMaxStateCount() const{ return m_maxStateCount; }
		uint32_t GetMaxAllocatedStateCount() const{ return m_maxAllocatedStateCount; }

	private:
		class StatesItem
		{
		public:
			StatesItem()
				: m_gpuResource(nullptr)
				, m_states(GfxResourceState::kMax)
			{
			}

			StatesItem(GfxGpuResource* gpuResource, GfxResourceStates states)
				: m_gpuResource(gpuResource)
				, m_states(states)
			{
			}
			
			GfxGpuResource*    m_gpuResource;
			GfxResourceStates  m_states;
		};

	private:
		uint32_t              m_maxStateCount;
		uint32_t              m_allocatedStateCount;
		HandleAllocator       m_handleAllocator;
		StatesItem*           m_stateItemArray;
		uint32_t              m_maxAllocatedStateCount;
	};

} // namespace SI
