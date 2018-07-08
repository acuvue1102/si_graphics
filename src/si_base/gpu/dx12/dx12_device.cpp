
#include "si_base/gpu\gfx_config.h"

#if SI_USE_DX12

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <comdef.h>

#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_base/memory/pool_allocator.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/dx12/dx12_command_queue.h"
#include "si_base/gpu/dx12/dx12_swap_chain.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/dx12/dx12_fence.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"

#include "si_base/gpu/dx12/dx12_device.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace SI
{
	size_t BaseDevice::s_descriptorSize[(int)GfxDescriptorHeapType::Max] = {};

	BaseDevice::BaseDevice()
		: Singleton(this)
		, m_initialized(false)
	{
	}

	BaseDevice::~BaseDevice()
	{
		Terminate();
	}

	int BaseDevice::Initialize(const GfxDeviceConfig& config)
	{
		if(m_initialized) return 0;

		m_config = config;
		
		m_objectAllocator = SI_NEW(PoolAllocatorEx);
		m_objectAllocator->InitializeEx(config.m_objectPoolSize);

		m_tempAllocator = SI_NEW(PoolAllocatorEx);
		m_tempAllocator->InitializeEx(config.m_tempPoolSize);

		int ret = 0;
		
		// create factory.
		//ComPtr<IDXGIFactory4> dxgiFactory;
		ret = InitializeFactory(m_dxgiFactory);
		if(ret != 0 || m_dxgiFactory == nullptr)
		{
			SI_ASSERT(0, "error InitializeFactory");
			return -1;
		}

		// create device.
		ret = InitializeDevice(m_dxgiFactory, m_device);
		if(ret != 0 || m_device == nullptr)
		{
			SI_ASSERT(0, "error InitializeDevice");
			return -1;
		}

		// descriptor heap sizeをあらかじめ保持しておく.
		for(int i=0; i<(int)GfxDescriptorHeapType::Max; ++i)
		{
			D3D12_DESCRIPTOR_HEAP_TYPE type = GetDx12DescriptorHeapType((GfxDescriptorHeapType)i);
			s_descriptorSize[i] = m_device->GetDescriptorHandleIncrementSize(type);
		}

		m_initialized = true;

		return 0;
	}

	int BaseDevice::Terminate()
	{
		if(!m_initialized) return 0;
		
#if 0 // DX12のmemory leakがあるときにこのコメントアウトを外すと詳細がわかる.
#if defined(_DEBUG)
		{
			ID3D12DebugDevice1* debugDevice;
			if (SUCCEEDED(m_device->QueryInterface(&debugDevice)))
			{
				debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL|D3D12_RLDO_IGNORE_INTERNAL);
				debugDevice->Release();
			}
		}
#endif // (_DEBUG)
#endif
		
		m_pipelineState.Reset();
		m_device.Reset();
		m_dxgiFactory.Reset();

		m_tempAllocator->TerminateEx();
		SI_DELETE(m_tempAllocator);

		m_objectAllocator->TerminateEx();
		SI_DELETE(m_objectAllocator);

		m_initialized = false;
		return 0;
	}
	
	size_t BaseDevice::GetDescriptorSize(GfxDescriptorHeapType type)
	{
		SI_ASSERT(s_descriptorSize[(int)type] != 0);
		return s_descriptorSize[(int)type];
	}
	
	int BaseDevice::InitializeFactory(ComPtr<IDXGIFactory4>& outDxgiFactory) const
	{
		HRESULT hr = 0;
		UINT dxgiFactoryFlags = 0;

	#if defined(_DEBUG)
		ComPtr<ID3D12Debug1> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			debugController->SetEnableSynchronizedCommandQueueValidation(TRUE);
			//debugController->SetEnableGPUBasedValidation(TRUE);
		
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	#endif // (_DEBUG)

		// DXGIFactoryを作る.
		hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&outDxgiFactory));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateDXGIFactory2: %s", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}
	
	int BaseDevice::InitializeDevice(
		ComPtr<IDXGIFactory4>& dxgiFactory,
		ComPtr<ID3D12Device>& outDevice) const
	{
		HRESULT hr = 0;

		// create Hardware Device. Ignore warp device.
		ComPtr<IDXGIAdapter1> adapter;
		for(UINT adapterId = 0;
			dxgiFactory->EnumAdapters1(adapterId, &adapter) != DXGI_ERROR_NOT_FOUND;
			++adapterId)
		{
			DXGI_ADAPTER_DESC1 adapterDesc;
			if(FAILED(adapter->GetDesc1(&adapterDesc)))
			{
				continue;
			}

			if( adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE )
			{
				continue; // warp adapter.
			}

			// Check if it is the adapter for DX12.
			hr = D3D12CreateDevice(
				adapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				__uuidof(ID3D12Device),
				nullptr);
			if(FAILED(hr))
			{
				continue; // dx12 is not supoorted.
			}

			hr = D3D12CreateDevice(
				adapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(&outDevice));

			if(SUCCEEDED(hr))
			{
				return 0;
			}

			SI_ASSERT(0, "error D3D12CreateDevice", _com_error(hr).ErrorMessage());
		}
		
		return -1;
	}
	
	BaseCommandQueue* BaseDevice::CreateCommandQueue()
	{
		BaseCommandQueue* cq = SI_NEW(BaseCommandQueue);
		if( cq->Initialize(*m_device.Get()) != 0 )
		{
			SI_ASSERT(0, "error CreateCommandQueue");
			delete cq;
			return nullptr;
		}
		return cq;
	}

	void BaseDevice::ReleaseCommandQueue(BaseCommandQueue* cq)
	{
		cq->Terminate();
		delete cq;
	}
	
	BaseSwapChain* BaseDevice::CreateSwapChain(
		const GfxDeviceConfig& config,
		BaseCommandQueue& commandQueue)
	{
		BaseSwapChain* sc = SI_NEW(BaseSwapChain);
		if( sc->Initialize(config, *m_device.Get(), *commandQueue.GetComPtrCommandQueue().Get(), *m_dxgiFactory.Get()) != 0 )
		{
			SI_ASSERT(0, "error CreateSwapChain");
			delete sc;
			return nullptr;
		}
		return sc;
	}

	void BaseDevice::ReleaseSwapChain(BaseSwapChain* sc)
	{
		SI_DELETE(sc);
	}

	BaseGraphicsCommandList* BaseDevice::CreateGraphicsCommandList()
	{
		BaseGraphicsCommandList* gcl = SI_NEW(BaseGraphicsCommandList);
		int ret = gcl->Initialize(*m_device.Get());
		if(ret != 0)
		{
			SI_ASSERT(0, "error InitializeCommandList");
			delete gcl;
			return nullptr;
		}

		return gcl;
	}

	void BaseDevice::ReleaseGraphicsCommandList(BaseGraphicsCommandList* gcl)
	{
		SI_DELETE(gcl);
	}

	BaseFence* BaseDevice::CreateFence()
	{
		BaseFence* f = SI_NEW(BaseFence);
		int ret = f->Initialize(*m_device.Get());
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateFence");
			SI_DELETE(f);
			return nullptr;
		}

		return f;

	}

	void BaseDevice::ReleaseFence(BaseFence* f)
	{
		SI_DELETE(f);
	}

	BaseFenceEvent* BaseDevice::CreateFenceEvent()
	{
		BaseFenceEvent* e = SI_NEW(BaseFenceEvent);
		int ret = e->Initialize();
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateFenceEvent");
			SI_DELETE(e);
			return nullptr;
		}

		return e;
	}

	void BaseDevice::ReleaseFenceEvent(BaseFenceEvent* e)
	{
		SI_DELETE(e);
	}
	
	BaseRootSignature* BaseDevice::CreateRootSignature(const GfxRootSignatureDesc& desc)
	{
		BaseRootSignature* s = SI_NEW(BaseRootSignature);
		int ret = s->Initialize(*this, desc);
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateRootSignature");
			SI_DELETE(s);
			return nullptr;
		}

		return s;
	}

	void BaseDevice::ReleaseRootSignature(BaseRootSignature* r)
	{
		SI_DELETE(r);
	}
	
	BaseGraphicsState* BaseDevice::CreateGraphicsState(const GfxGraphicsStateDesc& desc)
	{
		BaseGraphicsState* s = SI_NEW(BaseGraphicsState);
		int ret = s->Initialize(*m_device.Get(), desc);
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateGraphicsState");
			SI_DELETE(s);
			return nullptr;
		}

		return s;
	}

	void BaseDevice::ReleaseGraphicsState(BaseGraphicsState* s)
	{
		SI_DELETE(s);
	}
	
	BaseBuffer* BaseDevice::CreateBuffer(const GfxBufferDesc& desc)
	{
		BaseBuffer* b = SI_NEW(BaseBuffer);
		int ret = b->Initialize(*m_device.Get(), desc);
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateBuffer");
			SI_DELETE(b);
			return nullptr;
		}

		return b;
	}

	void BaseDevice::ReleaseBuffer(BaseBuffer* b)
	{
		SI_DELETE(b);
	}
	
	BaseTexture* BaseDevice::CreateTexture(const GfxTextureDesc& desc)
	{
		BaseTexture* t = SI_NEW(BaseTexture);
		int ret = t->Initialize(*m_device.Get(), desc);
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateTexture");
			SI_DELETE(t);
			return nullptr;
		}

		return t;
	}

	void BaseDevice::ReleaseTexture(BaseTexture* t)
	{
		SI_DELETE(t);
	}
	
	BaseDescriptorHeap* BaseDevice::CreateDescriptorHeap(const GfxDescriptorHeapDesc& desc)
	{
		BaseDescriptorHeap* d = SI_NEW(BaseDescriptorHeap);
		int ret = d->Initialize(*m_device.Get(), desc);
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateDescriptorHeap");
			SI_DELETE(d);
			return nullptr;
		}

		return d;
	}

	void BaseDevice::ReleaseDescriptorHeap(BaseDescriptorHeap* d)
	{
		SI_DELETE(d);
	}

	void BaseDevice::CreateRenderTargetView(
		BaseDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		BaseTexture& texture,
		const GfxRenderTargetViewDesc& desc)
	{
		GfxDescriptor descriptor = descriptorHeap.GetDescriptor(descriptorIndex);
		CreateRenderTargetView(descriptor, texture, desc);
	}

	void BaseDevice::CreateRenderTargetView(
		GfxDescriptor& descriptor,
		BaseTexture& texture,
		const GfxRenderTargetViewDesc& desc)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = texture.GetComPtrResource()->GetDesc().Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE dxDescriptor = {descriptor.GetCpuDescriptor().m_ptr};
		m_device->CreateRenderTargetView(texture.GetComPtrResource().Get(), &rtvDesc, dxDescriptor);
	}
	
	void BaseDevice::CreateDepthStencilView(
		BaseDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		BaseTexture& texture,
		const GfxDepthStencilViewDesc& desc)
	{
		GfxDescriptor descriptor = descriptorHeap.GetDescriptor(descriptorIndex);
		CreateDepthStencilView(
			descriptor,
			texture,
			desc);
	}
	
	void BaseDevice::CreateDepthStencilView(
		GfxDescriptor& descriptor,
		BaseTexture& texture,
		const GfxDepthStencilViewDesc& desc)
	{
		SI_ASSERT(texture.GetComPtrResource()->GetDesc().Format == DXGI_FORMAT_R32_TYPELESS);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format              = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension       = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice  = 0;
		dsvDesc.Flags               = D3D12_DSV_FLAG_NONE;

		D3D12_CPU_DESCRIPTOR_HANDLE dxDescriptor = {descriptor.GetCpuDescriptor().m_ptr};
		m_device->CreateDepthStencilView(texture.GetComPtrResource().Get(), &dsvDesc, dxDescriptor);
	}

	void BaseDevice::CreateShaderResourceView(
		BaseDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		BaseTexture& texture,
		const GfxShaderResourceViewDesc& desc)
	{
		GfxDescriptor descriptor = descriptorHeap.GetDescriptor(descriptorIndex);
		CreateShaderResourceView(descriptor, texture, desc);
	}

	void BaseDevice::CreateShaderResourceView(
		GfxDescriptor& descriptor,
		BaseTexture& texture,
		const GfxShaderResourceViewDesc& desc)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format                  = GetDx12Format(desc.m_format);
		srvDesc.ViewDimension           = GetDx12SrvDimension(desc.m_srvDimension);
		switch(desc.m_srvDimension)
		{
		case GfxDimension::Buffer:
		case GfxDimension::Texture1D:
			srvDesc.Texture1D.MipLevels = desc.m_miplevels;
			break;
		case GfxDimension::Texture1DArray:
			srvDesc.Texture1DArray.MipLevels = desc.m_miplevels;
			srvDesc.Texture1DArray.ArraySize = desc.m_arraySize;
			break;
		case GfxDimension::Texture2D:
			srvDesc.Texture2D.MipLevels = desc.m_miplevels;
			break;
		case GfxDimension::Texture2DArray:
			srvDesc.Texture2DArray.MipLevels = desc.m_miplevels;
			srvDesc.Texture2DArray.ArraySize = desc.m_arraySize;
			break;
		case GfxDimension::Texture2DMS:
		case GfxDimension::Texture2DMSArray:
			break;
		case GfxDimension::Texture3D:
			srvDesc.Texture3D.MipLevels = desc.m_miplevels;
			break;
		case GfxDimension::TextureCube:
			srvDesc.TextureCube.MipLevels = desc.m_miplevels;
			break;
		case GfxDimension::TextureCubeArray:
			srvDesc.TextureCubeArray.MipLevels = desc.m_miplevels;
			srvDesc.TextureCubeArray.NumCubes  = desc.m_arraySize;
			break;
		default:
			SI_ASSERT(0);
			break;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dxDescriptor = {descriptor.GetCpuDescriptor().m_ptr};
		m_device->CreateShaderResourceView(texture.GetComPtrResource().Get(), &srvDesc, dxDescriptor);
	}

	void BaseDevice::CreateSampler(
		BaseDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		const GfxSamplerDesc& desc)
	{
		GfxDescriptor descriptor = descriptorHeap.GetDescriptor(descriptorIndex);
		CreateSampler(
			descriptor,
			desc);
	}

	void BaseDevice::CreateSampler(
		GfxDescriptor& descriptor,
		const GfxSamplerDesc& desc)
	{
		D3D12_SAMPLER_DESC samplerDesc = {};		
		samplerDesc.Filter        = GetDx12Filter(desc.m_filter);
		samplerDesc.AddressU      = GetDx12TextureAddress(desc.m_addressU);
		samplerDesc.AddressV      = GetDx12TextureAddress(desc.m_addressV);
		samplerDesc.AddressW      = GetDx12TextureAddress(desc.m_addressW);
		samplerDesc.MipLODBias    = desc.m_mipLODBias;
		samplerDesc.MaxAnisotropy = desc.m_maxAnisotropy;
		samplerDesc.ComparisonFunc= GetDx12ComparisonFunc(desc.m_comparisonFunc);
		samplerDesc.BorderColor[0] = desc.m_borderColor[0];
		samplerDesc.BorderColor[1] = desc.m_borderColor[1];
		samplerDesc.BorderColor[2] = desc.m_borderColor[2];
		samplerDesc.BorderColor[3] = desc.m_borderColor[3];
		samplerDesc.MinLOD         = desc.m_minLOD;
		samplerDesc.MaxLOD         = desc.m_maxLOD;

		size_t descriptorSize = BaseDevice::GetDescriptorSize(GfxDescriptorHeapType::Sampler);
		
		D3D12_CPU_DESCRIPTOR_HANDLE dxDescriptor = {descriptor.GetCpuDescriptor().m_ptr};
		m_device->CreateSampler(&samplerDesc, dxDescriptor);
	}

	void BaseDevice::CreateConstantBufferView(
		BaseDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		const GfxConstantBufferViewDesc& desc)
	{
		descriptorHeap.CreateConstantBufferView(
			*m_device.Get(),
			descriptorIndex,
			desc);
	}

	void BaseDevice::CreateConstantBufferView(
		GfxDescriptor& descriptor,
		const GfxConstantBufferViewDesc& desc)
	{
		BaseBuffer* baseBuffer = desc.m_buffer->GetBaseBuffer();

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantDesc = {};
		constantDesc.BufferLocation = baseBuffer->GetLocation();
		constantDesc.SizeInBytes    = (UINT)baseBuffer->GetSize();

		size_t descriptorSize = BaseDevice::GetDescriptorSize(GfxDescriptorHeapType::CbvSrvUav);
		D3D12_CPU_DESCRIPTOR_HANDLE dxDescriptor = {descriptor.GetCpuDescriptor().m_ptr};
		m_device->CreateConstantBufferView(&constantDesc, dxDescriptor);
	}
	
	void BaseDevice::CopyDescriptors(
		uint32_t                 dstDescriptorRangeCount,
		const GfxCpuDescriptor*  dstDescriptorRangeStarts,
		const uint32_t*          dstDescriptorRangeSizes,
		uint32_t                 srcDescriptorRangeCount,
		const GfxCpuDescriptor*  srcDescriptorRangeStarts,
		const uint32_t*          srcDescriptorRangeSizes,
		GfxDescriptorHeapType    type)
	{
		static_assert(sizeof(GfxCpuDescriptor) == sizeof(D3D12_CPU_DESCRIPTOR_HANDLE), "size_error");

		const D3D12_CPU_DESCRIPTOR_HANDLE* dxDst = (const D3D12_CPU_DESCRIPTOR_HANDLE*)dstDescriptorRangeStarts;
		const D3D12_CPU_DESCRIPTOR_HANDLE* dxSrc = (const D3D12_CPU_DESCRIPTOR_HANDLE*)srcDescriptorRangeStarts;
		m_device.Get()->CopyDescriptors(
			dstDescriptorRangeCount, dxDst, dstDescriptorRangeSizes,
			srcDescriptorRangeCount, dxSrc, srcDescriptorRangeSizes,
			GetDx12DescriptorHeapType(type));
	}

} // namespace SI

#endif
