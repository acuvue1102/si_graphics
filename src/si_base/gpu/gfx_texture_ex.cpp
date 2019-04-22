
#include "si_base/gpu/gfx_texture_ex.h"

#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_core.h"
#include "si_base/gpu/gfx_texture.h"

#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_texture.h"

#include "si_base/gpu/gfx_dds.h"

namespace SI
{
	GfxTextureEx::GfxTextureEx()
		: m_srvDescriptor()
		, m_ref()
		, m_dimension(GfxDimension::Max)
	{
	}

	GfxTextureEx::~GfxTextureEx()
	{
		SI_ASSERT(!m_texture.IsValid());
		SI_ASSERT(!m_ref.IsValid());
	}
	
	uint32_t GfxTextureEx::GetWidth() const
	{
		return m_texture.GetWidth();
	}

	uint32_t GfxTextureEx::GetHeight() const
	{
		return m_texture.GetHeight();
	}

	uint32_t GfxTextureEx::GetDepth() const
	{
		return m_texture.GetDepth();
	}

	GfxFormat GfxTextureEx::GetFormat() const
	{
		return m_texture.GetFormat();
	}
	
	uint32_t GfxTextureEx::GetArraySize() const
	{
		return m_texture.GetArraySize();
	}
	
	uint32_t GfxTextureEx::GetMipLevels() const
	{
		return m_texture.GetMipLevels();
	}
	
	void* GfxTextureEx::GetNativeResource()
	{
		return m_texture.GetNativeResource();
	}
	
	////////////////////////////////////////////////////////////////////

	GfxTextureEx_Static::GfxTextureEx_Static()
	{
	}

	GfxTextureEx_Static::~GfxTextureEx_Static()
	{
		TerminateStatic();
	}
	
	void GfxTextureEx_Static::InitializeAs2DStatic(
		const char* name,
		uint32_t width,
		uint32_t height,
		GfxFormat format,
		const void* imageData,
		size_t imageDataSize,
		uint32_t mipLevel)
	{
		GfxDevice& device = *GfxDevice::GetInstance();

		GfxTextureDesc desc;
		desc.m_name           = name;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_format         = format;
		desc.m_mipLevels      = mipLevel;
		desc.m_dimension      = GfxDimension::Texture2D;
		desc.m_resourceStates = GfxResourceState::CopyDest;
		desc.m_resourceFlags  = GfxResourceFlag::None;
		desc.m_heapType       = GfxHeapType::Default;
		m_texture = device.CreateTexture(desc);

		m_dimension = desc.m_dimension;
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::Texture2D;
		srvDesc.m_format = desc.m_format;
		srvDesc.m_miplevels = mipLevel;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, m_texture, srvDesc);

		m_ref.Create();
		
		device.UploadTextureLater(
			m_texture,
			imageData,
			imageDataSize,
			GfxResourceState::CopyDest,
			GfxResourceState::PixelShaderResource);
	}
	
	int GfxTextureEx_Static::InitializeDDS(
		const char* name,
		const void* ddsBuffer,
		size_t ddsBufferSize,
		GfxDdsMetaData* outDdsMetaData)
	{
		GfxDdsMetaData ddsMetaData;

		int ret =LoadDdsFromMemory(ddsMetaData, ddsBuffer, ddsBufferSize);
		if(ret != 0)
		{
			return ret;
		}

		GfxDevice& device = *GfxDevice::GetInstance();

		GfxTextureDesc desc;
		desc.m_name           = name;
		desc.m_width          = ddsMetaData.m_width;
		desc.m_height         = ddsMetaData.m_height;
		desc.m_depth          = ddsMetaData.m_depth;
		desc.m_arraySize      = ddsMetaData.m_arraySize;
		desc.m_mipLevels      = ddsMetaData.m_mipLevel;
		desc.m_format         = ddsMetaData.m_format;
		desc.m_dimension      = ddsMetaData.m_dimension;
		desc.m_resourceStates = GfxResourceState::CopyDest;
		desc.m_resourceFlags  = GfxResourceFlag::None;
		desc.m_heapType       = GfxHeapType::Default;
		m_texture = device.CreateTexture(desc);
		
		m_dimension = desc.m_dimension;
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::Texture2D;
		srvDesc.m_format = desc.m_format;
		srvDesc.m_miplevels = desc.m_mipLevels;
		srvDesc.m_arraySize = desc.m_arraySize;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, m_texture, srvDesc);

		m_ref.Create();

		device.UploadTextureLater(
			m_texture,
			ddsMetaData.m_image,
			ddsMetaData.m_imageSise,
			GfxResourceState::CopyDest,
			GfxResourceState::PixelShaderResource);

		if(outDdsMetaData)
		{
			*outDdsMetaData = ddsMetaData;
		}

		return 0;
	}

	void GfxTextureEx_Static::TerminateStatic()
	{
		if(GetType() != GfxTextureExType::Static) return;

		if(m_texture.IsValid())
		{
			if(m_ref.ReleaseRef()==0)
			{
				GfxDevice::GetInstance()->ReleaseTexture(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Deallocate(m_srvDescriptor);
			}
			
			m_texture = GfxTexture();
			m_srvDescriptor = GfxDescriptor();
		}
	}
	////////////////////////////////////////////////////////////////////
			
	GfxTestureEx_Rt::GfxTestureEx_Rt()
		: _GfxTextureEx_Writable()
		, m_rtvDescriptor()
		, m_resourceStates(GfxResourceState::Common)
		, m_clearColor(0.0f)
	{
	}

	GfxTestureEx_Rt::~GfxTestureEx_Rt()
	{
		TerminateRt();
		SI_ASSERT(m_rtvDescriptor.GetCpuDescriptor().m_ptr==0);
	}

	void GfxTestureEx_Rt::InitializeAs2DRt(
		const char* name, uint32_t width, uint32_t height,
		GfxFormat format, const GfxColorRGBA& clearColor)
	{
		GfxDevice& device = *GfxDevice::GetInstance();

		GfxTextureDesc desc;
		desc.m_name           = name;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_format         = format;
		desc.m_dimension      = GfxDimension::Texture2D;
		desc.m_resourceStates = GfxResourceState::RenderTarget;
		desc.m_resourceFlags  = GfxResourceFlag::AllowRenderTarget;
		desc.m_heapType       = GfxHeapType::Default;
		desc.m_clearColor[0] = clearColor[0];
		desc.m_clearColor[1] = clearColor[1];
		desc.m_clearColor[2] = clearColor[2];
		desc.m_clearColor[3] = clearColor[3];
		
		m_clearColor = clearColor;
		SetInitialResourceStates(desc.m_resourceStates);

		m_texture = device.CreateTexture(desc);
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::Texture2D;
		srvDesc.m_format = format;
		srvDesc.m_miplevels = 1;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, m_texture, srvDesc);
		
		GfxRenderTargetViewDesc rtvDesc;
		m_rtvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::Rtv).Allocate(1);
		device.CreateRenderTargetView(m_rtvDescriptor, m_texture, rtvDesc);
		
		SI_RESOURCE_STATES_POOL().AllocateHandle(this);
				
		m_ref.Create();
	}

	void GfxTestureEx_Rt::TerminateRt()
	{
		if(GetType() != GfxTextureExType::Rt) return;
		
		if(m_texture.IsValid())
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				GfxDevice::GetInstance()->ReleaseTexture(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Deallocate(m_srvDescriptor);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::Rtv).Deallocate(m_rtvDescriptor);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = GfxTexture();
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
		SI_ASSERT(m_dsvDescriptor.GetCpuDescriptor().m_ptr==0);
	}
		
	void GfxTestureEx_DepthRt::InitializeAs2DDepthRt(
		const char* name, uint32_t width, uint32_t height,
		const float rtClearDepth)
	{
		GfxDevice& device = *GfxDevice::GetInstance();

		GfxTextureDesc desc;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_mipLevels      = 1;
		desc.m_format         = GfxFormat::R32_Typeless;
		desc.m_dimension      = GfxDimension::Texture2D;
		desc.m_resourceStates = GfxResourceState::DepthWrite;//GfxResourceState::GenericRead;
		desc.m_resourceFlags  = GfxResourceFlag::AllowDepthStencil;
		desc.m_heapType       = GfxHeapType::Default;
		desc.m_name           = name;
		desc.m_clearDepth     = rtClearDepth;
		m_texture = device.CreateTexture(desc);
		
		SetInitialResourceStates(desc.m_resourceStates);
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::Texture2D;
		srvDesc.m_format = GfxFormat::R32_Float;
		srvDesc.m_miplevels = 1;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, m_texture, srvDesc);
		
		GfxDepthStencilViewDesc dsvDesc;
		m_dsvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::Dsv).Allocate(1);
		device.CreateDepthStencilView(m_dsvDescriptor, m_texture, dsvDesc);

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
		
		if(m_texture.IsValid())
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				GfxDevice::GetInstance()->ReleaseTexture(m_texture);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Deallocate(m_srvDescriptor);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::Dsv).Deallocate(m_dsvDescriptor);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = GfxTexture();
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
		SI_ASSERT(m_rtvDescriptor.GetCpuDescriptor().m_ptr==0);
	}

	void GfxTestureEx_SwapChain::InitializeAsSwapChain(
		const char* name, uint32_t width, uint32_t height,
		void* nativeSwapChain, uint32_t swapChainIndex)
	{
		GfxDevice& device = *GfxDevice::GetInstance();

		SetInitialResourceStates(GfxResourceState::Common);

		m_texture = GfxTexture(SI_NEW(BaseTexture));
		m_texture.GetBaseTexture()->InitializeAsSwapChainTexture(width, height, nativeSwapChain, swapChainIndex);
		
		GfxRenderTargetViewDesc rtvDesc;
		m_rtvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::Rtv).Allocate(1);
		device.CreateRenderTargetView(m_rtvDescriptor, m_texture, rtvDesc);
				
		SI_RESOURCE_STATES_POOL().AllocateHandle(this);

		m_ref.Create();
	}

	void GfxTestureEx_SwapChain::TerminateSwapChain()
	{
		if(GetType() != GfxTextureExType::SwapChain) return;

		if(m_texture.IsValid())
		{
			GfxDevice& device = *GfxDevice::GetInstance();
			
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				SI_DELETE(m_texture.GetBaseTexture());
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::Rtv).Deallocate(m_rtvDescriptor);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = GfxTexture();
			m_rtvDescriptor = GfxDescriptor();
		}
	}
	

	///////////////////////////////////////////////////////////


	GfxTestureEx_Uav::GfxTestureEx_Uav()
	{
	}

	GfxTestureEx_Uav::~GfxTestureEx_Uav()
	{
		TerminateUav();
		SI_ASSERT(m_uavDescriptor.GetCpuDescriptor().m_ptr==0);
	}
		
	void GfxTestureEx_Uav::InitializeAs2DUav(
		const char* name, uint32_t width, uint32_t height,
		GfxFormat format)
	{
		GfxDevice& device = *GfxDevice::GetInstance();

		GfxTextureDesc desc;
		desc.m_width          = width;
		desc.m_height         = height;
		desc.m_mipLevels      = 1;
		desc.m_format         = format;
		desc.m_dimension      = GfxDimension::Texture2D;
		desc.m_resourceStates = GfxResourceState::UnorderedAccess;
		desc.m_resourceFlags  = GfxResourceFlag::AllowUnorderedAccess;
		desc.m_heapType       = GfxHeapType::Default;
		desc.m_name           = name;
		m_texture = device.CreateTexture(desc);
		
		SetInitialResourceStates(desc.m_resourceStates);
		
		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_srvDimension = GfxDimension::Texture2D;
		srvDesc.m_format = format;
		srvDesc.m_miplevels = 1;
		srvDesc.m_arraySize = 1;
		m_srvDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateShaderResourceView(m_srvDescriptor, m_texture, srvDesc);
		
		GfxUnorderedAccessViewDesc uavDesc;
		uavDesc.m_uavDimension = GfxDimension::Texture2D;
		uavDesc.m_format = format;
		uavDesc.m_mipslice   = 0;
		uavDesc.m_planeSlice = 0;
		m_uavDescriptor = SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Allocate(1);
		device.CreateUnorderedAccessView(m_uavDescriptor, m_texture, uavDesc);

		SI_RESOURCE_STATES_POOL().AllocateHandle(this);
				
		m_ref.Create();
	}

	void GfxTestureEx_Uav::TerminateUav()
	{
		if(GetType() != GfxTextureExType::Uav) return;
		
		if(m_texture.IsValid())
		{
			if(m_ref.ReleaseRef()==0)
			{
				SI_RESOURCE_STATES_POOL().DeallocateHandle(this);

				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Deallocate(m_uavDescriptor);
				SI_DESCRIPTOR_ALLOCATOR(GfxDescriptorHeapType::CbvSrvUav).Deallocate(m_srvDescriptor);
				GfxDevice::GetInstance()->ReleaseTexture(m_texture);
			}
			else
			{
				SetResourceStateHandle(kInvalidHandle);
			}
			
			m_texture = GfxTexture();
			m_srvDescriptor = GfxDescriptor();
			m_uavDescriptor = GfxDescriptor();
		}
	}

} // namespace SI
