
#include "si_base/gpu/gfx_descriptor_heap_pool.h"

#include "si_base/gpu/gfx_device.h"
#include "si_base/core/core.h"

namespace SI
{	
	GfxDescriptorHeapPool::GfxDescriptorHeapPool()
		: m_heapType(GfxDescriptorHeapType::Max)
		, m_usingHeapIndex(0)
	{
	}

	GfxDescriptorHeapPool::~GfxDescriptorHeapPool()
	{
		Terminate();
	}

	void GfxDescriptorHeapPool::Initialize(GfxDescriptorHeapType heapType, uint32_t maxHeapCount)
	{
		SI_ASSERT(heapType != GfxDescriptorHeapType::Max);
		SI_ASSERT(m_heapType == GfxDescriptorHeapType::Max);

		m_heapType = heapType;
		m_heapPool.Initialize(maxHeapCount);
		
		for(size_t i=0; i<ArraySize(m_usingHeaps); ++i)
		{
			m_usingHeaps[i].reserve(maxHeapCount);
		}
	}

	void GfxDescriptorHeapPool::Terminate()
	{
		if(m_heapType == GfxDescriptorHeapType::Max) return;

		GfxDevice& device = SI_DEVICE();

		for(size_t i=0; i<ArraySize(m_usingHeaps); ++i)
		{
			for(GfxDescriptorHeap* heap : m_usingHeaps[i])
			{
				m_heapPool.Deallocate(heap);
			}
			m_usingHeaps[i].clear();
		}

		uint32_t heapCount = m_heapPool.GetObjectCount();
		GfxDescriptorHeap* heaps = m_heapPool.GetObjectArray();
		for(uint32_t i=0; i<heapCount; ++i)
		{
			GfxDescriptorHeap& h = heaps[i];
			if(!h.IsValid()) continue;

			device.ReleaseDescriptorHeap(h);
		}

		m_heapPool.Terminate();
		m_heapType = GfxDescriptorHeapType::Max;
	}
	
	GfxDescriptorHeap* GfxDescriptorHeapPool::Allocate()
	{
		SI_ASSERT(m_heapType != GfxDescriptorHeapType::Max);

		MutexLocker locker(m_mutex);

		GfxDescriptorHeap* heap = m_heapPool.Allocate();
		if(!heap->IsValid())
		{
			static const uint32_t kDescriptorCountPerHeap = 1024;

			GfxDescriptorHeapDesc desc;
			desc.m_type = m_heapType;
			desc.m_flag = GfxDescriptorHeapFlag::ShaderVisible;
			desc.m_descriptorCount = kDescriptorCountPerHeap;

			*heap = SI_DEVICE().CreateDescriptorHeap(desc);
		}

		return heap;
	}

	void GfxDescriptorHeapPool::Deallocate(GfxDescriptorHeap* heap)
	{
		SI_ASSERT(m_heapType != GfxDescriptorHeapType::Max);

		MutexLocker locker(m_mutex);
		
		// すぐ開放せず、数フレーム待つためにリングバッファに入れる.
		m_usingHeaps[m_usingHeapIndex].push_back(heap);
	}
	
	void GfxDescriptorHeapPool::Flip()
	{
		MutexLocker locker(m_mutex);
		m_usingHeapIndex = (m_usingHeapIndex+1) % (uint32_t)ArraySize(m_usingHeaps);
		
		// 数フレーム待ったので開放.
		std::vector<GfxDescriptorHeap*>& readyHeaps = m_usingHeaps[m_usingHeapIndex];
		for(GfxDescriptorHeap* heap : readyHeaps)
		{
			m_heapPool.Deallocate(heap);
		}
		readyHeaps.clear();
	}

} // namespace SI
