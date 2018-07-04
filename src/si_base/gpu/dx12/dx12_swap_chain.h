#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include "si_base/gpu/dx12/dx12_fence.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_texture_ex.h"

namespace SI
{
	class BaseTexture;
	class BaseDescriptorHeap;

	class BaseSwapChain
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseSwapChain();
		~BaseSwapChain();
		
		int Initialize(
			const GfxDeviceConfig& config,
			ID3D12Device& device,
			ID3D12CommandQueue& commandQueue,
			IDXGIFactory4& f);

		int Terminate();

		int Present(uint32_t syncInterval);

		int Flip();

		UINT GetFrameIndex() const{ return m_frameIndex; }
		BaseTexture& GetSwapChainTexture();
		GfxCpuDescriptor GetSwapChainCpuDescriptor();
		GfxTestureEx_SwapChain& GetTexture();

	private:
		ComPtr<IDXGISwapChain3>           m_swapChain;
		GfxTestureEx_SwapChain*           m_textures;
		UINT                              m_bufferCount;
		UINT                              m_frameIndex;

		ID3D12CommandQueue*               m_commandQueue; // 参照しているだけ.
		BaseFence                         m_fence;
		BaseFenceEvent                    m_fenceEvent;
		uint64_t                          m_fenceValue;		
	};

} // namespace SI

#endif // SI_USE_DX12
