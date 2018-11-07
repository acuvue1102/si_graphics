
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
#include "si_base/gpu/gfx_utility.h"
#include "si_base/gpu/dx12/dx12_command_queue.h"
#include "si_base/gpu/dx12/dx12_swap_chain.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/dx12/dx12_fence.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_compute_state.h"
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"

#include "si_base/gpu/dx12/dx12_device.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace SI
{
	BaseDevice::BaseDevice()
		: Singleton(this)
		, m_initialized(false)
		, m_descriptorSize{}
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
			m_descriptorSize[i] = m_device->GetDescriptorHandleIncrementSize(type);
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
		SI_ASSERT(m_descriptorSize[(int)type] != 0);
		return m_descriptorSize[(int)type];
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
	
	BaseComputeState* BaseDevice::CreateComputeState(const GfxComputeStateDesc& desc)
	{
		BaseComputeState* s = SI_NEW(BaseComputeState);
		int ret = s->Initialize(*m_device.Get(), desc);
		if(ret != 0)
		{
			SI_ASSERT(0, "error CreateComputeState");
			SI_DELETE(s);
			return nullptr;
		}

		return s;
	}

	void BaseDevice::ReleaseComputeState(BaseComputeState* s)
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
	
	void BaseDevice::CreateUnorderedAccessView(
		BaseDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		BaseTexture& texture,
		const GfxUnorderedAccessViewDesc& desc)
	{
		GfxDescriptor descriptor = descriptorHeap.GetDescriptor(descriptorIndex);
		CreateUnorderedAccessView(descriptor, texture, desc);
	}

	void BaseDevice::CreateUnorderedAccessView(
		GfxDescriptor& descriptor,
		BaseTexture& texture,
		const GfxUnorderedAccessViewDesc& desc)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format                  = GetDx12Format(desc.m_format);
		uavDesc.ViewDimension           = GetDx12UavDimension(desc.m_uavDimension);
		switch(desc.m_uavDimension)
		{
		case GfxDimension::Buffer:
		case GfxDimension::Texture1D:
			uavDesc.Texture1D.MipSlice = desc.m_mipslice;
			break;
		case GfxDimension::Texture1DArray:
			uavDesc.Texture1DArray.MipSlice        = desc.m_mipslice;
			uavDesc.Texture1DArray.FirstArraySlice = desc.m_firstArraySlice;
			uavDesc.Texture1DArray.ArraySize       = desc.m_arraySize;
			break;
		case GfxDimension::Texture2D:
			uavDesc.Texture2D.MipSlice   = desc.m_mipslice;
			uavDesc.Texture2D.PlaneSlice = desc.m_planeSlice;
			break;
		case GfxDimension::Texture2DArray:
			uavDesc.Texture2DArray.ArraySize       = desc.m_arraySize;
			uavDesc.Texture2DArray.FirstArraySlice = desc.m_firstArraySlice;
			uavDesc.Texture2DArray.MipSlice        = desc.m_mipslice;
			uavDesc.Texture2DArray.PlaneSlice      = desc.m_planeSlice;
			break;
		case GfxDimension::Texture2DMS:
		case GfxDimension::Texture2DMSArray:
			break;
		case GfxDimension::Texture3D:
			uavDesc.Texture3D.FirstWSlice = desc.m_firstArraySlice;
			uavDesc.Texture3D.MipSlice    = desc.m_mipslice;
			uavDesc.Texture3D.WSize       = desc.m_wSize;
			break;
		default:
			SI_ASSERT(0);
			break;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dxDescriptor = {descriptor.GetCpuDescriptor().m_ptr};
		m_device->CreateUnorderedAccessView(texture.GetComPtrResource().Get(), nullptr, &uavDesc, dxDescriptor);
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

		size_t descriptorSize = BaseDevice::GetInstance()->GetDescriptorSize(GfxDescriptorHeapType::Sampler);
		
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

		size_t descriptorSize = BaseDevice::GetInstance()->GetDescriptorSize(GfxDescriptorHeapType::CbvSrvUav);
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
	
	int BaseDevice::CreateUploadBuffer(
		ComPtr<ID3D12Resource>& outBufferUploadHeap,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>&   outLayouts,
		BaseBuffer&             targetBuffer,
		const void*             srcBuffer,
		size_t                  srcBufferSize)
	{
		ID3D12Device& d3dDevice = *m_device.Get();
		ID3D12Resource& d3dResource = *targetBuffer.GetComPtrResource().Get();
		D3D12_RESOURCE_DESC resourceDesc = d3dResource.GetDesc();
						
		UINT subresourceNum = CalcSubresouceNum(
			resourceDesc.MipLevels,
			resourceDesc.DepthOrArraySize,
			resourceDesc.Dimension);
		SI_ASSERT(subresourceNum == 1);
			
		// 一時メモリアロケータからバッファを確保するstd::vector.
		outLayouts.clear();
		outLayouts.resize(subresourceNum);
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>&  layouts = outLayouts;
		//GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>   layouts(subresourceNum);

		UINT64 totalBytes = 0;
		d3dDevice.GetCopyableFootprints(
			&resourceDesc,
			0, subresourceNum, 0,
			&layouts[0], nullptr, nullptr, &totalBytes);
			
		if(srcBufferSize < totalBytes)
		{
			SI_ASSERT(0);
			return -1;
		}

		// アップロード用のバッファを用意する.
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.CreationNodeMask     = 1;
		heapProperties.VisibleNodeMask      = 1;
			
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.MipLevels          = 1;
		bufferDesc.Format             = DXGI_FORMAT_UNKNOWN;
		bufferDesc.Width              = totalBytes;
		bufferDesc.Height             = 1;
		bufferDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;
		bufferDesc.DepthOrArraySize   = 1;
		bufferDesc.SampleDesc.Count   = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Alignment          = 0;
		
		HRESULT hr = d3dDevice.CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&outBufferUploadHeap));

		BYTE* pData = nullptr;
		hr = outBufferUploadHeap->Map(0, NULL, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			SI_ASSERT(0, "error bufferUploadHeap->Map", _com_error(hr).ErrorMessage());
			return -1;
		}
		memcpy(pData, srcBuffer, srcBufferSize);
		outBufferUploadHeap->Unmap(0, NULL);

		return 0;
	}
		
	int BaseDevice::CreateUploadTexture(
		ComPtr<ID3D12Resource>& outTextureUploadHeap,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& outLayouts,
		BaseTexture& targetTexture,
		const void* srcBuffer,
		size_t srcBufferSize)
	{
		ID3D12Device& d3dDevice = *m_device.Get();
		ID3D12Resource& d3dResource = *targetTexture.GetComPtrResource().Get();
		D3D12_RESOURCE_DESC resourceDesc = d3dResource.GetDesc();

		UINT subresourceNum = CalcSubresouceNum(
			resourceDesc.MipLevels,
			resourceDesc.DepthOrArraySize,
			resourceDesc.Dimension);
			
		// 一時メモリアロケータからバッファを確保するstd::vector.
		//GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>   layouts(subresourceNum);
		outLayouts.clear();
		outLayouts.resize(subresourceNum);
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>&  layouts = outLayouts;
		GfxTempVector<UINT>                                 numRows(subresourceNum);
		GfxTempVector<UINT64>                               rowSizeInBytes(subresourceNum);

		UINT64 totalBytes = 0;
		d3dDevice.GetCopyableFootprints(
			&resourceDesc,
			0, subresourceNum, 0,
			&layouts[0], &numRows[0], &rowSizeInBytes[0], &totalBytes);


		// アップロード用のバッファを用意する.
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.CreationNodeMask     = 1;
		heapProperties.VisibleNodeMask      = 1;
			
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.MipLevels          = 1;
		bufferDesc.Format             = DXGI_FORMAT_UNKNOWN;
		bufferDesc.Width              = totalBytes;
		bufferDesc.Height             = 1;
		bufferDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;
		bufferDesc.DepthOrArraySize   = 1;
		bufferDesc.SampleDesc.Count   = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Alignment          = 0;
			
		HRESULT hr = d3dDevice.CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&outTextureUploadHeap));

		GfxTempVector<D3D12_SUBRESOURCE_DATA> sourcesData(subresourceNum);
		size_t bpp = GetDx12FormatBits(resourceDesc.Format);
		
		bool isBlock = IsBlockCompression(targetTexture.GetFormat());

		const uint8_t* tmpSrc = (const uint8_t*)srcBuffer;
		if(isBlock)
		{
			for(UINT i=0; i<subresourceNum; ++i)
			{
				uint32_t w = Max(1u, ((layouts[i].Footprint.Width  + 3u)/4u ));
				uint32_t h = Max(1u, ((layouts[i].Footprint.Height + 3u)/4u ));
				sourcesData[i].pData      = tmpSrc;
				sourcesData[i].RowPitch   = (w * 4 * 4 * bpp) / 8;
				sourcesData[i].SlicePitch = h * layouts[i].Footprint.Depth;
				tmpSrc += sourcesData[i].RowPitch * sourcesData[i].SlicePitch;
			}
		}
		else
		{
			for(UINT i=0; i<subresourceNum; ++i)
			{
				sourcesData[i].pData      = tmpSrc;
				sourcesData[i].RowPitch   = (layouts[i].Footprint.Width * bpp) / 8;
				sourcesData[i].SlicePitch = layouts[i].Footprint.Height * layouts[i].Footprint.Depth;
				tmpSrc += sourcesData[i].RowPitch * sourcesData[i].SlicePitch;
			}
		}
		SI_ASSERT((uintptr_t)tmpSrc <= (uintptr_t)srcBuffer + srcBufferSize);

		BYTE* pData = nullptr;
		hr = outTextureUploadHeap->Map(0, NULL, reinterpret_cast<void**>(&pData));
		if (FAILED(hr))
		{
			SI_ASSERT(0, "error textureUploadHeap->Map", _com_error(hr).ErrorMessage());
			return -1;
		}
				
		for (UINT i=0; i<subresourceNum; ++i)
		{
			if (rowSizeInBytes[i] > (SIZE_T)-1)
			{
				SI_ASSERT(0);
				return -1;
			}

			D3D12_MEMCPY_DEST destData =
			{
				pData + layouts[i].Offset,
				layouts[i].Footprint.RowPitch,
				layouts[i].Footprint.RowPitch * numRows[i]
			};

			MemcpySubresource(
				&destData,
				&sourcesData[i],
				(SIZE_T)rowSizeInBytes[i],
				numRows[i],
				layouts[i].Footprint.Depth);
		}
		outTextureUploadHeap->Unmap(0, NULL);

		return 0;
	}
	
	int BaseDevice::UploadBufferLater(
		BaseBuffer& targetBuffer,
		const void* srcBuffer,
		size_t      srcBufferSize)
	{
		ComPtr<ID3D12Resource> bufferUploadHeap;
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
		if(CreateUploadBuffer(
			bufferUploadHeap,
			layouts,
			targetBuffer,
			srcBuffer,
			srcBufferSize) != 0)
		{
			return -1;
		}
		
		// upload用のバッファだけ作ってpoolに貯めて、後でコピーする.
		m_uploadPool.AddBuffer(
			targetBuffer,
			std::move(bufferUploadHeap),
			std::move(layouts));

		return 0;
	}

	int BaseDevice::UploadTextureLater(
		BaseTexture& targetTexture,
		const void*  srcBuffer,
		size_t       srcBufferSize)
	{
		ComPtr<ID3D12Resource>                            textureUploadHeap;
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
		if(CreateUploadTexture(
			textureUploadHeap,
			layouts,
			targetTexture,
			srcBuffer,
			srcBufferSize) != 0)
		{
			return -1;
		}
		
		// upload用のバッファだけ作ってpoolに貯めて、後でコピーする.
		m_uploadPool.AddTexture(
			targetTexture,
			std::move(textureUploadHeap),
			std::move(layouts));

		return 0;
	}
	
	int BaseDevice::FlushUploadPool(BaseGraphicsCommandList& commandList)
	{
		m_uploadPool.Flush(commandList);
		return 0;
	}

} // namespace SI

#endif
