#pragma once

#include "si_base/core/singleton.h"
#include "si_base/gpu/gfx_config.h"
#include "si_base/memory/handle_allocator.h"
#include "si_base/gpu/gfx_descriptor_heap.h"

namespace SI
{
	class GfxDescriptorAllocator
	{
	public:
		GfxDescriptorAllocator();
		~GfxDescriptorAllocator();

		void Initialize(GfxDescriptorHeapType type, uint32_t maxDescriptorCount);
		void Terminate();

		GfxDescriptor Allocate(uint32_t count);
		void Deallocate(GfxDescriptor descriptor);		

	private:
		uint32_t              m_maxDescriptorCount;
		uint32_t              m_allocatedDescriptorCount;
		GfxDescriptorHeap     m_heap;
		GfxDescriptor         m_baseDescriptor;
		GfxDescriptor         m_currentDescriptor;
		GfxDescriptor         m_endDescriptor;
		size_t                m_typeSize;
	};

} // namespace SI
