
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
		desc.m_name = name;
		desc.m_heapType = GfxHeapType::Default;
		desc.m_bufferSizeInByte = size;
		desc.m_resourceStates   = GfxResourceState::CopyDest;
		desc.m_resourceFlags    = GfxResourceFlag::None;
		m_buffer = device.CreateBuffer(desc);

		m_ref.Create();
	}

	void GfxBufferEx::TerminateAsStatic()
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
		desc.m_name = name;
		desc.m_heapType = GfxHeapType::Upload;
		desc.m_bufferSizeInByte = AlignUp(size, (size_t)256); // 256byteの倍数じゃないとダメ.
		desc.m_resourceStates = GfxResourceState::GenericRead;
		m_buffer = device.CreateBuffer(desc);
		m_mapPtr = m_buffer->Map(0);
		
		GfxBuffer gfxBuffer(m_buffer);

		GfxConstantBufferViewDesc cbvDesc;
		cbvDesc.m_buffer = &gfxBuffer;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateConstantBufferView(m_srvDescriptor, cbvDesc);

		m_ref.Create();
	}

	void GfxBufferEx_Constant::TerminateAsConstant()
	{
		if(GetType() != GfxBufferExType::Constant) return;

		if(m_buffer)
		{
			if(m_ref.ReleaseRef()==0)
			{
				m_buffer->Unmap(0);
				m_mapPtr = nullptr;
				SI_BASE_DEVICE().ReleaseBuffer(m_buffer);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Deallocate(m_srvDescriptor);
			}
			
			m_buffer = nullptr;
			m_srvDescriptor = GfxDescriptor();
		}
	}
	

	////////////////////////////////////////////////////////////////////
	
	GfxBufferEx_Index::GfxBufferEx_Index()
		: m_format(GfxFormat::Max)
	{
	}

	GfxBufferEx_Index::~GfxBufferEx_Index()
	{
		SI_ASSERT(m_format == GfxFormat::Max);
	}
		
	void GfxBufferEx_Index::InitializeAsIndex( const char* name, size_t indexCount, bool is16bit)
	{
		BaseDevice& device = SI_BASE_DEVICE();
		
		GfxBufferDesc desc;
		desc.m_name = name;
		desc.m_heapType = GfxHeapType::Default;
		desc.m_bufferSizeInByte = indexCount * (is16bit? 2 : 4);
		desc.m_resourceStates   = GfxResourceState::CopyDest;
		desc.m_resourceFlags    = GfxResourceFlag::None;
		m_buffer = device.CreateBuffer(desc);
		m_format = is16bit? GfxFormat::R16_Uint : GfxFormat::R32_Uint;

		m_ref.Create();
	}

	void GfxBufferEx_Index::TerminateAsIndex()
	{
		if(GetType() != GfxBufferExType::Index) return;

		if(m_buffer)
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_BASE_DEVICE().ReleaseBuffer(m_buffer);
			}
			
			m_buffer = nullptr;
		}

		m_format = GfxFormat::Max;
	}

	////////////////////////////////////////////////////////////////////
	
	GfxBufferEx_Vertex::GfxBufferEx_Vertex()
		: m_stride(0)
		, m_offset(0)
	{
	}

	GfxBufferEx_Vertex::~GfxBufferEx_Vertex()
	{
		SI_ASSERT(m_stride == 0);
	}
		
	void GfxBufferEx_Vertex::InitializeAsVertex( const char* name, size_t vertexCount, size_t stride, size_t offset)
	{
		BaseDevice& device = SI_BASE_DEVICE();
		
		GfxBufferDesc desc;
		desc.m_name = name;
		desc.m_heapType = GfxHeapType::Default;
		desc.m_bufferSizeInByte = vertexCount * stride;
		desc.m_resourceStates   = GfxResourceState::CopyDest;
		desc.m_resourceFlags    = GfxResourceFlag::None;
		m_buffer = device.CreateBuffer(desc);
		m_stride = stride;

		m_ref.Create();
	}

	void GfxBufferEx_Vertex::TerminateAsVertex()
	{
		if(GetType() != GfxBufferExType::Vertex) return;

		if(m_buffer)
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_BASE_DEVICE().ReleaseBuffer(m_buffer);
			}
			
			m_buffer = nullptr;
		}

		m_stride = 0;
	}

} // namespace SI
