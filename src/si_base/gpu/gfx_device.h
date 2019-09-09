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

		GfxComputeState CreateComputeState(const GfxComputeStateDesc& desc);
		void ReleaseComputeState(GfxComputeState& state);

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

		GfxRaytracingStateDesc CreateRaytracingStateDesc();
		void ReleaseRaytracingStateDesc(GfxRaytracingStateDesc& raytracingStateDesc);

		GfxRaytracingState CreateRaytracingState(GfxRaytracingStateDesc& desc);
		void ReleaseRaytracingState(GfxRaytracingState& raytracingState);
		
		GfxRaytracingScene CreateRaytracingScene();
		void ReleaseRaytracingScene(GfxRaytracingScene& scene);

		GfxRaytracingShaderTables CreateRaytracingShaderTables(GfxRaytracingShaderTablesDesc& desc);
		void ReleaseRaytracingShaderTables(GfxRaytracingShaderTables& shaderTable);

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

		void CreateDepthStencilView(
			GfxDescriptor& descriptor,
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

		void CreateShaderResourceView(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			GfxBuffer& buffer,
			const GfxShaderResourceViewDesc& desc);

		void CreateShaderResourceView(
			GfxDescriptor& descriptor,
			GfxBuffer& buffer,
			const GfxShaderResourceViewDesc& desc);
		
		void CreateUnorderedAccessView(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			GfxTexture& texture,
			const GfxUnorderedAccessViewDesc& desc);
		
		void CreateUnorderedAccessView(
			GfxDescriptor& descriptor,
			GfxTexture& texture,
			const GfxUnorderedAccessViewDesc& desc);

		void CreateSampler(
			GfxDescriptorHeap& descriptorHeap,
			uint32_t descriptorIndex,
			const GfxSamplerDesc& desc);

		void CreateSampler(
			GfxDescriptor& descriptor,
			const GfxSamplerDesc& desc);

		void CreateConstantBufferView(
			GfxDescriptorHeap& descriptorHeap,
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

		void CopyDescriptorsSimple(
			uint32_t                 descriptorCount,
			GfxCpuDescriptor         dstDescriptorRangeStart,
			GfxCpuDescriptor         srcDescriptorRangeStart,
			GfxDescriptorHeapType    type);
		
		// upload用のバッファを作って登録、Flushまで転送はしない.
		int UploadBufferLater(
			GfxBuffer&              targetBuffer,
			const void*             srcBuffer,
			size_t                  srcBufferSize,
			GfxResourceStates       before,
			GfxResourceStates       after);

		int UploadTextureLater(
			GfxTexture&             targetTexture,
			const void*             srcBuffer,
			size_t                  srcBufferSize,
			GfxResourceStates       before,
			GfxResourceStates       after);

		// upload用のバッファを転送する.
		int FlushUploadPool(GfxGraphicsCommandList& commandList);
	public:
		BaseDevice* GetBaseDevice(){ return m_base; }
		const BaseDevice* GetBaseDevice() const{ return m_base; }
		
		PoolAllocatorEx* GetObjectAllocator();
		PoolAllocatorEx* GetTempAllocator();

		void* GetNative();

	private:
		BaseDevice* m_base;
	};

} // namespace SI


#define SI_DEVICE()                 (*SI::GfxDevice::GetInstance())
#define SI_DEVICE_TEMP_ALLOCATOR()	(*SI::GfxDevice::GetInstance()->GetTempAllocator())
#define SI_DEVICE_OBJ_ALLOCATOR()	(*SI::GfxDevice::GetInstance()->GetObjectAllocator())
