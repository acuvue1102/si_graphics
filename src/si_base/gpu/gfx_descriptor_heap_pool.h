#pragma once

#include <vector>
#include "si_base/core/assert.h"
#include "si_base/core/constant.h"
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/memory/object_pool.h"
#include "si_base/concurency/mutex.h"

namespace SI
{
	class GfxDescriptorHeapPool
	{
	public:
		GfxDescriptorHeapPool();
		~GfxDescriptorHeapPool();

		void Initialize(GfxDescriptorHeapType heapType, uint32_t maxHeapCount);
		void Terminate();

		GfxDescriptorHeap* Allocate();
		void Deallocate(GfxDescriptorHeap* heap);

		void Flip();

	private:
		GfxDescriptorHeapType           m_heapType;
		ObjectPool<GfxDescriptorHeap>   m_heapPool;
		std::vector<GfxDescriptorHeap*> m_usingHeaps[3];
		uint32_t                        m_usingHeapIndex;
		Mutex                           m_mutex;
	};
} // namespace SI
