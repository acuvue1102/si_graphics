
#include "si_base/gpu\gfx_config.h"

#if SI_USE_DX12

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <comdef.h>

#include "si_base/core/base.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/dx12/dx12_command_queue.h"
#include "si_base/gpu/dx12/dx12_swap_chain.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/dx12/dx12_fence.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_buffer.h"

#include "si_base/gpu/dx12/dx12_device.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace SI
{
	BaseDevice::BaseDevice()
		: m_initialized(false)
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

		return 0;
	}

	int BaseDevice::Terminate()
	{
		if(!m_initialized) return 0;
		
		m_pipelineState.Reset();
		m_device.Reset();
		m_dxgiFactory.Reset();

		m_initialized = false;
		return 0;
	}
	
	int BaseDevice::InitializeFactory(ComPtr<IDXGIFactory4>& outDxgiFactory) const
	{
		HRESULT hr = 0;
		UINT dxgiFactoryFlags = 0;

	#if defined(_DEBUG)
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		
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
		delete sc;
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
		delete gcl;
	}

	BaseFence* BaseDevice::CreateFence()
	{
		BaseFence* f = SI_NEW(BaseFence);
		int ret = f->Initialize(*m_device.Get());
		if(ret != 0)
		{
			SI_ASSERT(0, "error InitializeCommandList");
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
			SI_ASSERT(0, "error InitializeCommandList");
			SI_DELETE(e);
			return nullptr;
		}

		return e;
	}

	void BaseDevice::ReleaseFenceEvent(BaseFenceEvent* e)
	{
		SI_DELETE(e);
	}
	
	BaseRootSignature* BaseDevice::CreateRootSignature()
	{
		BaseRootSignature* s = SI_NEW(BaseRootSignature);
		int ret = s->Initialize(*m_device.Get());
		if(ret != 0)
		{
			SI_ASSERT(0, "error InitializeRootSignature");
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
			SI_ASSERT(0, "error InitializeGraphicsState");
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
			SI_ASSERT(0, "error InitializeBuffer");
			SI_DELETE(b);
			return nullptr;
		}

		return b;
	}

	void BaseDevice::ReleaseBuffer(BaseBuffer* b)
	{
		SI_DELETE(b);
	}

} // namespace SI

#endif
