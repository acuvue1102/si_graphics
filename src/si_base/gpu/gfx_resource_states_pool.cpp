
#include "si_base/gpu/gfx_resource_states_pool.h"
#include "si_base/core/new_delete.h"
#include "si_base/core/basic_function.h"

#include "si_base/gpu/gfx_gpu_resource.h"

namespace SI
{
	GfxResourceStatesPool::GfxResourceStatesPool()
		: m_maxStateCount(0)
		, m_allocatedStateCount(0)
		, m_handleAllocator()
		, m_stateItemArray(nullptr)
		, m_maxAllocatedStateCount(0)
	{
	}

	GfxResourceStatesPool::~GfxResourceStatesPool()
	{
		Terminate();

		SI_ASSERT(m_stateItemArray==nullptr);
	}

	void GfxResourceStatesPool::Initialize(uint32_t maxStateCount)
	{
		SI_ASSERT(m_stateItemArray==nullptr);

		m_maxStateCount = maxStateCount;
		m_allocatedStateCount = 0;

		m_handleAllocator.Initialize(m_maxStateCount);

		m_stateItemArray = SI_NEW_ARRAY(StatesItem, maxStateCount);
	}

	void GfxResourceStatesPool::Terminate()
	{
		if(m_stateItemArray)
		{
			SI_ASSERT(m_allocatedStateCount==0, "開放忘れ");

			SI_DELETE_ARRAY(m_stateItemArray);
			m_stateItemArray = nullptr;

			m_handleAllocator.Terminate();

			m_maxStateCount = 0;
			m_allocatedStateCount = 0;
		}
	}

	void GfxResourceStatesPool::AllocateHandle(GfxGpuResource* gpuResource)
	{
		SI_ASSERT(m_stateItemArray);

		uint32_t handle = m_handleAllocator.Allocate();
		SI_ASSERT(handle < m_maxStateCount);

		gpuResource->SetResourceStateHandle(handle);
		GfxResourceStates initialStates = gpuResource->GetInitialResourceStates();

		m_stateItemArray[handle] = StatesItem(gpuResource, initialStates);

		m_maxAllocatedStateCount = Max(m_maxAllocatedStateCount, handle+1);

		++m_allocatedStateCount;
	}
	
	void GfxResourceStatesPool::DeallocateHandle(GfxGpuResource* gpuResource)
	{
		SI_ASSERT(m_stateItemArray);

		uint32_t handle = gpuResource->GetResourceStateHandle();
		SI_ASSERT(handle < m_maxStateCount);

		m_handleAllocator.Deallocate(handle);
		gpuResource->SetResourceStateHandle(kInvalidHandle);

		--m_allocatedStateCount;
	}
	
	void GfxResourceStatesPool::SetResourceStates(uint32_t resourceStateHanlde, GfxResourceStates states)
	{
		SI_ASSERT(resourceStateHanlde != kInvalidHandle);
		SI_ASSERT(m_stateItemArray[resourceStateHanlde].m_states != GfxResourceState::Max);
		m_stateItemArray[resourceStateHanlde].m_states = states;
	}

	void GfxResourceStatesPool::SetResourceStates(const GfxGpuResource* gpuResource, GfxResourceStates states)
	{
		SetResourceStates(gpuResource->GetResourceStateHandle(), states);
	}

	GfxResourceStates GfxResourceStatesPool::GetResourceStates(uint32_t resourceStateHanlde)
	{
		SI_ASSERT(resourceStateHanlde != kInvalidHandle);
		SI_ASSERT(m_stateItemArray[resourceStateHanlde].m_states != GfxResourceState::Max);
		return m_stateItemArray[resourceStateHanlde].m_states;
	}

	GfxResourceStates GfxResourceStatesPool::GetResourceStates(const GfxGpuResource* gpuResource)
	{
		return GetResourceStates(gpuResource->GetResourceStateHandle());
	}
	
	GfxGpuResource* GfxResourceStatesPool::GetGpuResource(uint32_t resourceStateHanlde)
	{
		SI_ASSERT(resourceStateHanlde != kInvalidHandle);
		SI_ASSERT(m_stateItemArray[resourceStateHanlde].m_states != GfxResourceState::Max);
		return m_stateItemArray[resourceStateHanlde].m_gpuResource;
	}

} // namespace SI
