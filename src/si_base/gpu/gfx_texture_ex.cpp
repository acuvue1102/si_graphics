
#include "si_base/gpu/gfx_texture_ex.h"

#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_core.h"
#include "si_base/gpu/gfx_texture.h"

#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_texture.h"

namespace SI
{
	GfxTextureEx::GfxTextureEx()
		: m_texture(nullptr)
		, m_srvDescriptor()
		, m_ref()
		, m_dimension(GfxDimension::kMax)
	{
	}

	GfxTextureEx::~GfxTextureEx()
	{
		SI_ASSERT(m_texture==nullptr);
		SI_ASSERT(!m_ref.IsValid());
	}

	void GfxTextureEx::InitializeAs2DStatic(
		const char* name, uint32_t width, uint32_t height, GfxFormat format)
	{
		BaseDevice& device = SI_BASE_DEVICE();

		GfxTextureDesc desc;
		desc.m_name           = name;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_format         = format;
		desc.m_dimension      = GfxDimension::kTexture2D;
		desc.m_resourceStates = GfxResourceState::kCopyDest;
		desc.m_resourceFlags  = GfxResourceFlag::kNone;
		desc.m_heapType       = GfxHeapType::kDefault;

		m_texture = device.CreateTexture(desc);
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::kTexture2D;
		srvDesc.m_format = format;
		srvDesc.m_miplevels = 1;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, *m_texture, srvDesc);

		m_ref.Create();
	}

	void GfxTextureEx::TerminateStatic()
	{
		if(GetType() != GfxTextureExType::Static) return;

		if(m_texture)
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_BASE_DEVICE().ReleaseTexture(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Deallocate(m_srvDescriptor);
			}
			
			m_texture = nullptr;
			m_srvDescriptor = GfxDescriptor();
		}
	}
	
	uint32_t GfxTextureEx::GetWidth() const
	{
		return m_texture->GetWidth();
	}

	uint32_t GfxTextureEx::GetHeight() const
	{
		return m_texture->GetHeight();
	}

	uint32_t GfxTextureEx::GetDepth() const
	{
		return m_texture->GetDepth();
	}
	
	void* GfxTextureEx::GetNativeResource()
	{
		return m_texture->GetNativeResource();
	}
	
	////////////////////////////////////////////////////////////////////
			
	GfxTestureEx_Rt::GfxTestureEx_Rt()
		: _GfxTextureEx_Writable()
		, m_rtvDescriptor()
		, m_resourceStates(GfxResourceState::kCommon)
		, m_clearColor(0.0f)
	{
	}

	GfxTestureEx_Rt::~GfxTestureEx_Rt()
	{
		TerminateRt();
	}

	void GfxTestureEx_Rt::InitializeAs2DRt(
		const char* name, uint32_t width, uint32_t height,
		GfxFormat format, const GfxColorRGBA& clearColor)
	{
		BaseDevice& device = SI_BASE_DEVICE();

		GfxTextureDesc desc;
		desc.m_name           = name;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_format         = format;
		desc.m_dimension      = GfxDimension::kTexture2D;
		desc.m_resourceStates = GfxResourceState::kRenderTarget;
		desc.m_resourceFlags  = GfxResourceFlag::kAllowRenderTarget;
		desc.m_heapType       = GfxHeapType::kDefault;
		desc.m_clearColor[0] = clearColor[0];
		desc.m_clearColor[1] = clearColor[1];
		desc.m_clearColor[2] = clearColor[2];
		desc.m_clearColor[3] = clearColor[3];
		
		m_clearColor = clearColor;
		SetInitialResourceStates(desc.m_resourceStates);

		m_texture = device.CreateTexture(desc);
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::kTexture2D;
		srvDesc.m_format = format;
		srvDesc.m_miplevels = 1;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, *m_texture, srvDesc);
		
		GfxRenderTargetViewDesc rtvDesc;
		m_rtvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kRtv).Allocate(1);
		device.CreateRenderTargetView(m_rtvDescriptor, *m_texture, rtvDesc);
		
		SI_RESOURCE_STATES_POOL().AllocateHandle(this);
				
		m_ref.Create();
	}

	void GfxTestureEx_Rt::TerminateRt()
	{
		if(GetType() != GfxTextureExType::Rt) return;
		
		if(m_texture)
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				SI_BASE_DEVICE().ReleaseTexture(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Deallocate(m_srvDescriptor);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kRtv).Deallocate(m_rtvDescriptor);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = nullptr;
			m_srvDescriptor = GfxDescriptor();
			m_rtvDescriptor = GfxDescriptor();
		}
	}

	////////////////////////////////////////////////////////////////////
	
	GfxTestureEx_DepthRt::GfxTestureEx_DepthRt()
	{
	}

	GfxTestureEx_DepthRt::~GfxTestureEx_DepthRt()
	{
		TerminateDepthRt();
	}
		
	void GfxTestureEx_DepthRt::InitializeAs2DDepthRt(
		const char* name, uint32_t width, uint32_t height,
		const float rtClearDepth)
	{
		BaseDevice& device = SI_BASE_DEVICE();

		GfxTextureDesc desc;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_mipLevels      = 1;
		desc.m_format         = GfxFormat::kR32_Typeless;
		desc.m_dimension      = GfxDimension::kTexture2D;
		desc.m_resourceStates = GfxResourceState::kGenericRead;
		desc.m_resourceFlags  = GfxResourceFlag::kAllowDepthStencil;
		desc.m_heapType       = GfxHeapType::kDefault;
		desc.m_name           = "Depth";
		desc.m_clearDepth     = rtClearDepth;
		m_texture = device.CreateTexture(desc);
		
		SetInitialResourceStates(desc.m_resourceStates);
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::kTexture2D;
		srvDesc.m_format = GfxFormat::kR32_Float;
		srvDesc.m_miplevels = 1;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, *m_texture, srvDesc);
		
		GfxDepthStencilViewDesc dsvDesc;
		m_dsvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kDsv).Allocate(1);
		device.CreateDepthStencilView(m_dsvDescriptor, *m_texture, dsvDesc);

		SI_RESOURCE_STATES_POOL().AllocateHandle(this);
				
		m_ref.Create();
	}
		
	void GfxTestureEx_DepthRt::InitializeAs2DDepthStencilRt(
		const char* name, uint32_t width, uint32_t height,
		float clearDepth, uint8_t clearStencil)
	{
		SI_ASSERT(0, "未実装");
	}

	void GfxTestureEx_DepthRt::TerminateDepthRt()
	{
		if(GetType() != GfxTextureExType::DepthRt) return;
		
		if(m_texture)
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				SI_BASE_DEVICE().ReleaseTexture(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kCbvSrvUav).Deallocate(m_srvDescriptor);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kDsv).Deallocate(m_dsvDescriptor);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = nullptr;
			m_srvDescriptor = GfxDescriptor();
			m_dsvDescriptor = GfxDescriptor();
		}
	}

	///////////////////////////////////////////////////////////
	
	GfxTestureEx_SwapChain::GfxTestureEx_SwapChain()
	{
	}

	GfxTestureEx_SwapChain::~GfxTestureEx_SwapChain()
	{
		TerminateSwapChain();
	}

	void GfxTestureEx_SwapChain::InitializeAsSwapChain(
		const char* name, uint32_t width, uint32_t height,
		void* nativeSwapChain, uint32_t swapChainIndex)
	{
		BaseDevice& device = SI_BASE_DEVICE();

		SetInitialResourceStates(GfxResourceState::kCommon);

		m_texture = SI_NEW(BaseTexture);
		m_texture->InitializeAsSwapChainTexture(width, height, nativeSwapChain, swapChainIndex);
		
		GfxRenderTargetViewDesc rtvDesc;
		m_rtvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kRtv).Allocate(1);
		device.CreateRenderTargetView(m_rtvDescriptor, *m_texture, rtvDesc);
				
		SI_RESOURCE_STATES_POOL().AllocateHandle(this);

		m_ref.Create();
	}

	void GfxTestureEx_SwapChain::TerminateSwapChain()
	{
		if(GetType() != GfxTextureExType::SwapChain) return;

		if(m_texture)
		{
			BaseDevice& device = SI_BASE_DEVICE();
			
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				SI_DELETE(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::kRtv).Deallocate(m_rtvDescriptor);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = nullptr;
			m_rtvDescriptor = GfxDescriptor();
		}
	}
} // namespace SI
