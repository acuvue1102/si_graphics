
#include "gpu/gfx_config.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "base/base.h"
#include "dx12_texture.h"

namespace SI
{
	BaseTexture::BaseTexture()
	{
	}

	BaseTexture::~BaseTexture()
	{
		Terminate();
	}
	
	int BaseTexture::InitializeAsSwapChainTexture(
		const GfxDeviceConfig& config,
		ID3D12Device& device,
		IDXGISwapChain3& swapChain,
		int swapChainBufferId)
	{
		static const D3D12_DESCRIPTOR_HEAP_DESC kDesc =
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,   //D3D12_DESCRIPTOR_HEAP_TYPE Type;
			1,                                // UINT NumDescriptors;
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE,  //D3D12_DESCRIPTOR_HEAP_FLAGS Flags;
			0,                                //UINT NodeMask;
		};

		SetWidth(config.m_width);
		SetHeight(config.m_height);
		SetDepth(1);

		SI_ASSERT(m_descHeap == nullptr);
		HRESULT hr = device.CreateDescriptorHeap(&kDesc, IID_PPV_ARGS(&m_descHeap));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateDescriptorHeap", _com_error(hr).ErrorMessage());
			return -1;
		}
			
		D3D12_CPU_DESCRIPTOR_HANDLE handle(m_descHeap->GetCPUDescriptorHandleForHeapStart());
		SetPtr(handle.ptr);
		
		SI_ASSERT(m_resource == nullptr);
		hr = swapChain.GetBuffer(swapChainBufferId, IID_PPV_ARGS(&m_resource));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error swapChain->GetBuffer", _com_error(hr).ErrorMessage());
			return -1;
		}

		device.CreateRenderTargetView(m_resource.Get(), nullptr, handle);

		return 0;
	}

	int BaseTexture::Terminate()
	{
		m_resource.Reset();
		m_descHeap.Reset();
		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
