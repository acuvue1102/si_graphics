
#include "si_base/gpu/gfx_descriptor_allocator.h"

#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/gfx_device.h"

namespace SI
{
	GfxDescriptorAllocator::GfxDescriptorAllocator()
		: m_maxDescriptorCount(0)
		, m_allocatedDescriptorCount(0)
		, m_typeSize(0)
	{
	}

	GfxDescriptorAllocator::~GfxDescriptorAllocator()
	{
		Terminate();
	}

	void GfxDescriptorAllocator::Initialize(GfxDescriptorHeapType type, uint32_t maxDescriptorCount)
	{
		m_typeSize = BaseDevice::GetDescriptorSize(type);
		m_maxDescriptorCount = maxDescriptorCount;

		GfxDescriptorHeapDesc desc;
		desc.m_descriptorCount = maxDescriptorCount;
		desc.m_type = type;
		desc.m_flag = GfxDescriptorHeapFlag::None;
		m_heap = SI_DEVICE().CreateDescriptorHeap(desc);
		m_baseDescriptor = GfxDescriptor(m_heap.GetCpuDescriptor(0), m_heap.GetGpuDescriptor(0));
		m_endDescriptor = m_baseDescriptor + m_typeSize * m_maxDescriptorCount;
		m_currentDescriptor = m_baseDescriptor;
	}
	
	void GfxDescriptorAllocator::Terminate()
	{
		if(m_heap.IsValid())
		{
			SI_DEVICE().ReleaseDescriptorHeap(m_heap);
			m_baseDescriptor = GfxDescriptor();
			m_endDescriptor = GfxDescriptor();
			m_maxDescriptorCount = 0;
			m_typeSize = 0;
		}
	}
	
	GfxDescriptor GfxDescriptorAllocator::Allocate(uint32_t count)
	{
		MutexLocker locker(m_mutex);

		if(m_maxDescriptorCount <= m_allocatedDescriptorCount+count)
		{
			SI_ASSERT(0);
			return GfxDescriptor();
		}
		m_allocatedDescriptorCount += count;

		GfxDescriptor ret = m_currentDescriptor;
		m_currentDescriptor += m_typeSize * count;
		return ret;
	}

	void GfxDescriptorAllocator::Deallocate(GfxDescriptor descriptor)
	{
		MutexLocker locker(m_mutex);
		// TODO: きちんと開放する...
	}

} // namespace SI
