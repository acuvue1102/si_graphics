#pragma once

#include <cstdint>
#include "si_base/core/singleton.h"
#include "si_base/gpu/gfx_declare.h"
#include "si_base/gpu/gfx_descriptor_allocator.h"

namespace SI
{
	class PoolAllocatorEx;

	class BaseDevice;

	class GfxDevice : public Singleton<GfxDevice>
	{
	public:
		GfxDevice();
		~GfxDevice();

		int Initialize(const GfxDeviceConfig& config);
		int Terminate();

	public:
		GfxCommandQueue CreateCommandQueue();
		void ReleaseCommandQueue(GfxCommandQueue& commandQueue);

		GfxSwapChain CreateSwapChain(
			const GfxDeviceConfig& config,
			GfxCommandQueue& commandQueue);
		void ReleaseSwapChain(GfxSwapChain& swapChain);

		GfxGraphicsCommandList CreateGraphicsCommandList();
		void ReleaseGraphicsCommandList(GfxGraphicsCommandList& commandList);

		GfxGraphicsState CreateGraphicsState(const GfxGraphicsStateDesc& desc);
		void ReleaseGraphicsState(GfxGraphicsState& state);

		GfxFence CreateFence();
		void ReleaseFence(GfxFence& fence);

		GfxFenceEvent CreateFenceEvent();
		void ReleaseFenceEvent(GfxFenceEvent& event);

		GfxRootSignature CreateRootSignature(const GfxRootSignatureDesc& desc);
		void ReleaseRootSignature(GfxRootSignature& signature);

		GfxBuffer CreateBuffer(const GfxBufferDesc& desc);
		void ReleaseBuffer(GfxBuffer& buffer);

		GfxTexture CreateTexture(const GfxTextureDesc& desc);
		void ReleaseTexture(GfxTexture& texture);
		
		GfxDescriptorHeap CreateDescriptorHeap(const GfxDescriptorHeapDesc& desc);
		void ReleaseDescriptorHeap(GfxDescriptorHeap& descriptorHeap);

		void CreateRenderTargetView(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			GfxTexture& texture,
			const GfxRenderTargetViewDesc& desc);

		void CreateRenderTargetView(
			GfxDescriptor& descriptor,
			GfxTexture& texture,
			const GfxRenderTargetViewDesc& desc);

		void CreateDepthStencilView(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			GfxTexture& texture,
			const GfxDepthStencilViewDesc& desc);
		
		void CreateShaderResourceView(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			GfxTexture& texture,
			const GfxShaderResourceViewDesc& desc);
		
		void CreateShaderResourceView(
			GfxDescriptor& descriptor,
			GfxTexture& texture,
			const GfxShaderResourceViewDesc& desc);

		void CreateSampler(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			const GfxSamplerDesc& desc);

		void CreateConstantBufferView(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			const GfxConstantBufferViewDesc& desc);

	public:
		BaseDevice* GetBaseDevice(){ return m_base; }
		const BaseDevice* GetBaseDevice() const{ return m_base; }
		
		PoolAllocatorEx* GetObjectAllocator();
		PoolAllocatorEx* GetTempAllocator();

	private:
		BaseDevice* m_base;
	};

} // namespace SI


#define SI_DEVICE()                 (*SI::GfxDevice::GetInstance())
#define SI_DEVICE_TEMP_ALLOCATOR()	(*SI::GfxDevice::GetInstance()->GetTempAllocator())
#define SI_DEVICE_OBJ_ALLOCATOR()	(*SI::GfxDevice::GetInstance()->GetObjectAllocator())
