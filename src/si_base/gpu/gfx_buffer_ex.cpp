
#include "si_base/gpu/gfx_buffer_ex.h"

#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_core.h"
#include "si_base/gpu/gfx_buffer.h"

#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_buffer.h"

namespace SI
{
	GfxBufferEx::GfxBufferEx()
		: m_buffer(nullptr)
		, m_ref()
	{
	}

	GfxBufferEx::~GfxBufferEx()
	{
		SI_ASSERT(m_buffer==nullptr);
		SI_ASSERT(!m_ref.IsValid());
	}

	void GfxBufferEx::InitializeAsStatic(const char* name, size_t size)
	{
		BaseDevice& device = SI_BASE_DEVICE();
		
		GfxBufferDesc desc;
		desc.m_name = "textureConstant";
		desc.m_heapType = GfxHeapType::kDefault;
		desc.m_bufferSizeInByte = size;
		desc.m_resourceStates   = GfxResourceState::kCopyDest;
		desc.m_resourceFlags    = GfxResourceFlag::kNone;
		m_buffer = device.CreateBuffer(desc);

		m_ref.Create();
	}

	void GfxBufferEx::TerminateStatic()
	{
		if(GetType() != GfxBufferExType::Static) return;

		if(m_buffer)
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_BASE_DEVICE().ReleaseBuffer(m_buffer);
			}
			
			m_buffer = nullptr;
		}
	}
	
	size_t GfxBufferEx::GetSize() const
	{
		return m_buffer->GetSize();
	}
	
	void* GfxBufferEx::GetNativeResource()
	{
		return m_buffer->GetNativeResource();
	}

	////////////////////////////////////////////////////////////////////

	GfxBufferEx_Constant::GfxBufferEx_Constant()
		: m_srvDescriptor()
	{
	}

	GfxBufferEx_Constant::~GfxBufferEx_Constant()
	{
		SI_ASSERT(m_srvDescriptor.GetCpuDescriptor().m_ptr == 0);
	}

	void GfxBufferEx_Constant::InitializeAsConstant(const char* name, size_t size)
	{
		BaseDevice& device = SI_BASE_DEVICE();
		
		GfxBufferDesc desc;
		desc.m_name = "textureConstant";
		desc.m_heapType = GfxHeapType::kUpload;
		desc.m_bufferSizeInByte = AlignUp(size, (size_t)256); // 256byteの倍数じゃないとダメ.
		desc.m_resourceStates = GfxResourceState::kGenericRead;
		m_buffer = device.CreateBuffer(desc);
		m_mapPtr = m_buffer->Map(0);
		
		GfxBuffer gfxBuffer(m_buffer);

		GfxConstantBufferViewDesc cbvDesc;
		cbvDesc.m_buffer = &gfxBuffer;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Allocate(1);
		device.CreateConstantBufferView(m_srvDescriptor, cbvDesc);

		m_ref.Create();
	}

	void GfxBufferEx_Constant::TerminateConstant()
	{
		if(GetType() != GfxBufferExType::Constant) return;

		if(m_buffer)
		{
			if(m_ref.ReleaseRef()==0)
			{
				m_buffer->Unmap(0);
				m_mapPtr = nullptr;
				SI_BASE_DEVICE().ReleaseBuffer(m_buffer);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Deallocate(m_srvDescriptor);
			}
			
			m_buffer = nullptr;
			m_srvDescriptor = GfxDescriptor();
		}
	}
	
} // namespace SI
