#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>

struct IDXGIFactory4;

namespace SI
{
	class BaseCommandQueue;
	class BaseSwapChain;
	class BaseGraphicsCommandList;
	class BaseFence;
	class BaseFenceEvent;
	class BaseRootSignature;
	class BaseGraphicsState;
	class BaseBuffer;
	struct GfxGraphicsStateDesc;
	struct GfxBufferDesc;

	class BaseDevice
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseDevice();
		~BaseDevice();
		
		int Initialize(const GfxDeviceConfig& config);
		int Terminate();

	public:
		BaseCommandQueue* CreateCommandQueue();
		void ReleaseCommandQueue(BaseCommandQueue* cq);

		BaseSwapChain* CreateSwapChain(
			const GfxDeviceConfig& config,
			BaseCommandQueue& commandQueue);
		void ReleaseSwapChain(BaseSwapChain* sc);

		BaseGraphicsCommandList* CreateGraphicsCommandList();
		void ReleaseGraphicsCommandList(BaseGraphicsCommandList* gcl);

		BaseFence* CreateFence();
		void ReleaseFence(BaseFence* f);

		BaseFenceEvent* CreateFenceEvent();
		void ReleaseFenceEvent(BaseFenceEvent* e);

		BaseRootSignature* CreateRootSignature();
		void ReleaseRootSignature(BaseRootSignature* r);

		BaseGraphicsState* CreateGraphicsState(const GfxGraphicsStateDesc& desc);
		void ReleaseGraphicsState(BaseGraphicsState* s);

		BaseBuffer* CreateBuffer(const GfxBufferDesc& desc);
		void ReleaseBuffer(BaseBuffer* b);

	private:
		int InitializeFactory(ComPtr<IDXGIFactory4>& outDxgiFactory) const;

		int InitializeDevice(
			ComPtr<IDXGIFactory4>& dxgiFactory,
			ComPtr<ID3D12Device>& outDevice) const;

	private:
		GfxDeviceConfig                   m_config;
		ComPtr<IDXGIFactory4>             m_dxgiFactory; // 持ちたくないが、DX12ではdeviceから参照出来ないので持つ.
		ComPtr<ID3D12Device>              m_device;
		ComPtr<ID3D12PipelineState>       m_pipelineState;
		bool                              m_initialized;
	};

} // namespace SI

#endif
