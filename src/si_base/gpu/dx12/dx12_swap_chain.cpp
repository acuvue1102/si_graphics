
#include "si_base/gpu/dx12/dx12_swap_chain.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"

namespace SI
{
	BaseSwapChain::BaseSwapChain()
		: m_textures(nullptr)
		, m_bufferCount(0)
		, m_frameIndex(0)
		, m_commandQueue(nullptr)
		, m_device(nullptr)
	{
	}

	BaseSwapChain::~BaseSwapChain()
	{
		Terminate();
	}
	
	int BaseSwapChain::Initialize(
		const GfxDeviceConfig& config,
		ID3D12Device& device,
		ID3D12CommandQueue& commandQueue,
		IDXGIFactory4& dxgiFactory)
	{
		SI_ASSERT(config.m_hWnd != nullptr);
		HRESULT hr = 0;

		m_bufferCount = config.m_bufferCount;

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_bufferCount;
		swapChainDesc.Width = config.m_width;
		swapChainDesc.Height = config.m_height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		HWND hwnd = (HWND)config.m_hWnd;

		ComPtr<IDXGISwapChain1> swapChain;
		hr = dxgiFactory.CreateSwapChainForHwnd(
			&commandQueue,
			hwnd,//(HWND)config.m_hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain);
		m_commandQueue = &commandQueue;

		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateSwapChainForHwnd", _com_error(hr).ErrorMessage());
			return -1;
		}

		// full screen is not supported.
		hr = dxgiFactory.MakeWindowAssociation((HWND)config.m_hWnd, DXGI_MWA_NO_ALT_ENTER);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error MakeWindowAssociation", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = swapChain.As(&m_swapChain);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error swapChain.As", _com_error(hr).ErrorMessage());
			return -1;
		}

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		
		SI_ASSERT(m_textures == nullptr);
		m_textures          = SI_NEW_ARRAY(GfxTestureEx_SwapChain, config.m_bufferCount);

		for (UINT bufferId = 0; bufferId < (UINT)config.m_bufferCount; bufferId++)
		{
			m_textures[bufferId].InitializeAsSwapChain(
				"swapChain",
				config.m_width, config.m_height,
				swapChain.Get(), bufferId);
		}
		
		m_fenceValue = 0;
		m_fence.Initialize(device);
		m_fenceEvent.Initialize();

		m_device = &device;

		return 0;
	}


	int BaseSwapChain::Terminate()
	{
		m_fenceEvent.Terminate();
		m_fence.Terminate();
		m_fenceValue = 0;
		m_commandQueue = nullptr;

		for(uint32_t i=0; i<m_bufferCount; ++i)
		{
			m_textures[i].TerminateSwapChain();
		}

		m_bufferCount = 0;
		m_frameIndex = 0;
		SI_DELETE_ARRAY(m_textures);
		m_textures = nullptr;
		m_swapChain.Reset();
		return 0;
	}
	
	int BaseSwapChain::Present(uint32_t syncInterval)
	{
		HRESULT hr = m_swapChain->Present(1, 0);
		if(FAILED(hr))
		{
			HRESULT hr2 = m_device->GetDeviceRemovedReason();
			SI_ASSERT(0, "error m_swapChain->Present: %s: %s", _com_error(hr).ErrorMessage(), _com_error(hr2).ErrorMessage() );
			return -1;
		}

		return 0;
	}
		
	int BaseSwapChain::Flip()
	{
		ID3D12Fence* fence = m_fence.GetComPtrFence().Get();
		uint64_t fenceValue = m_fenceValue;
		HRESULT hr = m_commandQueue->Signal(fence, fenceValue);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_commandQueue->Signal: %s", _com_error(hr).ErrorMessage());
			return -1;
		}
		++m_fenceValue;
		
		uint64_t waitingFenceValue = fenceValue;
		//if(2<m_bufferCount && m_bufferCount < waitingFenceValue )
		//{
		//	waitingFenceValue -= (m_bufferCount - 2);
		//}
		if(fence->GetCompletedValue() < waitingFenceValue)
		{
			// GPU is still working for previous frame.
			hr = fence->SetEventOnCompletion(waitingFenceValue, m_fenceEvent.GetHandle());
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error fence->SetEventOnCompletion: %s", _com_error(hr).ErrorMessage());
				return -1;
			}

			// wait event.
			WaitForSingleObject(m_fenceEvent.GetHandle(), INFINITE);
		}
		
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		return 0;
	}
		
	int BaseSwapChain::Wait()
	{
		ID3D12Fence* fence = m_fence.GetComPtrFence().Get();
		volatile uint64_t fenceValue = m_fenceValue;
		HRESULT hr = m_commandQueue->Signal(fence, fenceValue);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_commandQueue->Signal: %s", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = m_commandQueue->Wait(fence, fenceValue);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_commandQueue->Signal: %s", _com_error(hr).ErrorMessage());
			return -1;
		}
		
		while(fence->GetCompletedValue() < fenceValue);

		return 0;
	}
	
	BaseTexture& BaseSwapChain::GetSwapChainTexture()
	{
		return *m_textures[m_frameIndex].GetBaseTexture();
	}
	
	GfxCpuDescriptor BaseSwapChain::GetSwapChainCpuDescriptor()
	{
		return m_textures[m_frameIndex].GetRtvDescriptor().GetCpuDescriptor();
	}
	
	GfxTestureEx_SwapChain& BaseSwapChain::GetTexture()
	{
		return m_textures[m_frameIndex];
	}

} // namespace SI

#endif // SI_USE_DX12
