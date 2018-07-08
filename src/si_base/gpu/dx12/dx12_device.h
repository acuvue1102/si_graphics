#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include "si_base/core/singleton.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/dx12/dx12_declare.h"
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"

struct IDXGIFactory4;

namespace SI
{
	class PoolAllocatorEx;
	struct GfxCpuDescriptor;

	class BaseDevice : public Singleton<BaseDevice>
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

		BaseRootSignature* CreateRootSignature(const GfxRootSignatureDesc& desc);
		void ReleaseRootSignature(BaseRootSignature* r);

		BaseGraphicsState* CreateGraphicsState(const GfxGraphicsStateDesc& desc);
		void ReleaseGraphicsState(BaseGraphicsState* s);

		BaseBuffer* CreateBuffer(const GfxBufferDesc& desc);
		void ReleaseBuffer(BaseBuffer* b);

		BaseTexture* CreateTexture(const GfxTextureDesc& desc);
		void ReleaseTexture(BaseTexture* t);

		BaseDescriptorHeap* CreateDescriptorHeap(const GfxDescriptorHeapDesc& desc);
		void ReleaseDescriptorHeap(BaseDescriptorHeap* d);
		
		void CreateRenderTargetView(
			BaseDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			BaseTexture& texture,
			const GfxRenderTargetViewDesc& desc);

		void CreateRenderTargetView(
			GfxDescriptor& descriptor,
			BaseTexture& texture,
			const GfxRenderTargetViewDesc& desc);

		void CreateDepthStencilView(
			BaseDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			BaseTexture& texture,
			const GfxDepthStencilViewDesc& desc);

		void CreateDepthStencilView(
			GfxDescriptor& descriptor,
			BaseTexture& texture,
			const GfxDepthStencilViewDesc& desc);

		void CreateShaderResourceView(
			BaseDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			BaseTexture& texture,
			const GfxShaderResourceViewDesc& desc);

		void CreateShaderResourceView(
			GfxDescriptor& descriptor,
			BaseTexture& texture,
			const GfxShaderResourceViewDesc& desc);
		
		void CreateSampler(
			BaseDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			const GfxSamplerDesc& desc);

		void CreateSampler(
			GfxDescriptor& descriptor,
			const GfxSamplerDesc& desc);

		void CreateConstantBufferView(
			BaseDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			const GfxConstantBufferViewDesc& desc);

		void CreateConstantBufferView(
			GfxDescriptor& descriptor,
			const GfxConstantBufferViewDesc& desc);

		void CopyDescriptors(
			uint32_t                 dstDescriptorRangeCount,
			const GfxCpuDescriptor*  dstDescriptorRangeStarts,
			const uint32_t*          dstDescriptorRangeSizes,
			uint32_t                 srcDescriptorRangeCount,
			const GfxCpuDescriptor*  srcDescriptorRangeStarts,
			const uint32_t*          srcDescriptorRangeSizes,
			GfxDescriptorHeapType    type);

	public:
		PoolAllocatorEx* GetObjectAllocator(){ return m_objectAllocator; }
		PoolAllocatorEx* GetTempAllocator()  { return m_tempAllocator; }

	public:
		ComPtr<ID3D12Device>& GetComPtrDevice()
		{
			return m_device;
		}
		
	public:
		static size_t GetDescriptorSize(GfxDescriptorHeapType type);

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
		PoolAllocatorEx*                  m_objectAllocator;
		PoolAllocatorEx*                  m_tempAllocator;
		bool                              m_initialized;

		static size_t                     s_descriptorSize[(int)GfxDescriptorHeapType::Max];
	};

} // namespace SI

#define SI_BASE_DEVICE() (*SI::BaseDevice::GetInstance())

#endif
