﻿
#include "si_base/gpu/gfx_device.h"

#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/gfx_command_queue.h"
#include "si_base/gpu/gfx_graphics_command_list.h"
#include "si_base/gpu/gfx_texture.h"
#include "si_base/gpu/gfx_swap_chain.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_compute_state.h"
#include "si_base/gpu/gfx_fence.h"
#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_raytracing_state.h"
#include "si_base/gpu/gfx_raytracing_geometry.h"
#include "si_base/gpu/gfx_raytracing_shader_table.h"

namespace SI
{
	GfxDevice::GfxDevice()
		: Singleton(this)
		, m_base(nullptr)
	{
	}

	GfxDevice::~GfxDevice()
	{
		Terminate();
	}

	int GfxDevice::Initialize(const GfxDeviceConfig& config)
	{
		if(m_base) return 0;

		m_base = SI_NEW(BaseDevice);
		return m_base->Initialize(config);
	}

	int GfxDevice::Terminate()
	{
		if(!m_base) return 0;

		int ret = m_base->Terminate();
		SI_DELETE(m_base);
		m_base = nullptr;

		return ret;
	}
		
	GfxCommandQueue GfxDevice::CreateCommandQueue()
	{
		GfxCommandQueue cq(m_base->CreateCommandQueue());
		return cq;
	}
	
	void GfxDevice::ReleaseCommandQueue(GfxCommandQueue& commandQueue)
	{
		m_base->ReleaseCommandQueue(commandQueue.GetBaseCommandQueue());
		commandQueue = GfxCommandQueue(nullptr);
	}
	
	GfxSwapChain GfxDevice::CreateSwapChain(
		const GfxDeviceConfig& config,
		GfxCommandQueue& commandQueue)
	{
		GfxSwapChain sc(m_base->CreateSwapChain(config, *commandQueue.GetBaseCommandQueue()));
		return sc;
	}
	
	void GfxDevice::ReleaseSwapChain(GfxSwapChain& swapChain)
	{
		m_base->ReleaseSwapChain(swapChain.GetBaseSwapChain());
		swapChain = GfxSwapChain(nullptr);
	}
	
	GfxGraphicsCommandList GfxDevice::CreateGraphicsCommandList()
	{
		GfxGraphicsCommandList gcl(m_base->CreateGraphicsCommandList());
		return gcl;
	}

	void GfxDevice::ReleaseGraphicsCommandList(GfxGraphicsCommandList& commandList)
	{
		m_base->ReleaseGraphicsCommandList(commandList.GetBaseGraphicsCommandList());
		commandList = GfxGraphicsCommandList();
	}
	
	GfxGraphicsState GfxDevice::CreateGraphicsState(const GfxGraphicsStateDesc& desc)
	{
		GfxGraphicsState s(m_base->CreateGraphicsState(desc));
		return s;
	}

	void GfxDevice::ReleaseGraphicsState(GfxGraphicsState& state)
	{
		m_base->ReleaseGraphicsState(state.GetBaseGraphicsState());
		state = GfxGraphicsState();
	}
	
	GfxComputeState GfxDevice::CreateComputeState(const GfxComputeStateDesc& desc)
	{
		GfxComputeState s(m_base->CreateComputeState(desc));
		return s;
	}

	void GfxDevice::ReleaseComputeState(GfxComputeState& state)
	{
		m_base->ReleaseComputeState(state.GetBaseComputeState());
		state = GfxComputeState();
	}
	
	GfxFence GfxDevice::CreateFence()
	{
		GfxFence f(m_base->CreateFence());
		return f;
	}

	void GfxDevice::ReleaseFence(GfxFence& fence)
	{
		m_base->ReleaseFence(fence.GetBaseFence());
		fence = GfxFence();
	}

	GfxFenceEvent GfxDevice::CreateFenceEvent()
	{
		GfxFenceEvent e(m_base->CreateFenceEvent());
		return e;
	}

	void GfxDevice::ReleaseFenceEvent(GfxFenceEvent& event)
	{
		m_base->ReleaseFenceEvent(event.GetBaseFenceEvent());
		event = GfxFenceEvent();
	}

	GfxRootSignature GfxDevice::CreateRootSignature(const GfxRootSignatureDesc& desc)
	{
		GfxRootSignature s(m_base->CreateRootSignature(desc));
		return s;
	}

	void GfxDevice::ReleaseRootSignature(GfxRootSignature& signature)
	{
		m_base->ReleaseRootSignature(signature.GetBaseRootSignature());
		signature = GfxRootSignature();
	}

	
	GfxBuffer GfxDevice::CreateBuffer(const GfxBufferDesc& desc)
	{
		GfxBuffer b(m_base->CreateBuffer(desc));
		return b;
	}

	void GfxDevice::ReleaseBuffer(GfxBuffer& buffer)
	{
		m_base->ReleaseBuffer(buffer.GetBaseBuffer());
		buffer = GfxBuffer();
	}
	
	GfxTexture GfxDevice::CreateTexture(const GfxTextureDesc& desc)
	{
		GfxTexture t(m_base->CreateTexture(desc));
		return t;
	}

	void GfxDevice::ReleaseTexture(GfxTexture& texture)
	{
		m_base->ReleaseTexture(texture.GetBaseTexture());
		texture = GfxTexture();
	}

	GfxTexture GfxDevice::CreateTextureWICAndUpload(
		const char* name, const void* buffer, size_t bufferSize)
	{
		GfxTexture t(m_base->CreateTextureWICAndUpload(name, buffer, bufferSize));
		return t;
	}
		
	GfxDescriptorHeap GfxDevice::CreateDescriptorHeap(const GfxDescriptorHeapDesc& desc)
	{
		GfxDescriptorHeap d(m_base->CreateDescriptorHeap(desc));
		return d;
	}

	void GfxDevice::ReleaseDescriptorHeap(GfxDescriptorHeap& descriptorHeap)
	{
		m_base->ReleaseDescriptorHeap(descriptorHeap.GetBaseDescriptorHeap());
		descriptorHeap = GfxDescriptorHeap();
	}

	GfxRaytracingStateDesc GfxDevice::CreateRaytracingStateDesc()
	{
		return GfxRaytracingStateDesc(m_base->CreateRaytracingStateDesc());
	}

	void GfxDevice::ReleaseRaytracingStateDesc(GfxRaytracingStateDesc& raytracingStateDesc)
	{
		m_base->ReleaseRaytracingStateDesc(raytracingStateDesc.GetBase());
		raytracingStateDesc = GfxRaytracingStateDesc();
	}

	GfxRaytracingState GfxDevice::CreateRaytracingState(GfxRaytracingStateDesc& desc)
	{
		return GfxRaytracingState(m_base->CreateRaytracingState(desc.GetBase()));
	}

	void GfxDevice::ReleaseRaytracingState(GfxRaytracingState& raytracingState)
	{
		m_base->ReleaseRaytracingState(&raytracingState.GetBase());
		raytracingState = GfxRaytracingState();
	}

	GfxRaytracingScene GfxDevice::CreateRaytracingScene()
	{
		return GfxRaytracingScene(m_base->CreateRaytracingScene());
	}

	void GfxDevice::ReleaseRaytracingScene(GfxRaytracingScene& scene)
	{
		m_base->ReleaseRaytracingScene(scene.GetBase());
		scene = GfxRaytracingScene();
	}

	GfxRaytracingShaderTables GfxDevice::CreateRaytracingShaderTables(GfxRaytracingShaderTablesDesc& desc)
	{
		return GfxRaytracingShaderTables(m_base->CreateRaytracingShaderTables(desc));
	}

	void GfxDevice::ReleaseRaytracingShaderTables(GfxRaytracingShaderTables& shaderTable)
	{
		m_base->ReleaseRaytracingShaderTables(shaderTable.GetBase());
		shaderTable = GfxRaytracingShaderTables();
	}
	
	void GfxDevice::CreateRenderTargetView(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		GfxTexture& texture,
		const GfxRenderTargetViewDesc& desc)
	{
		m_base->CreateRenderTargetView(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			*texture.GetBaseTexture(),
			desc);
	}
	
	void GfxDevice::CreateRenderTargetView(
		GfxDescriptor& descriptor,
		GfxTexture& texture,
		const GfxRenderTargetViewDesc& desc)
	{
		m_base->CreateRenderTargetView(
			descriptor,
			*texture.GetBaseTexture(),
			desc);
	}

	void GfxDevice::CreateDepthStencilView(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		GfxTexture& texture,
		const GfxDepthStencilViewDesc& desc)
	{
		m_base->CreateDepthStencilView(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			*texture.GetBaseTexture(),
			desc);
	}

	void GfxDevice::CreateDepthStencilView(
		GfxDescriptor& descriptor,
		GfxTexture& texture,
		const GfxDepthStencilViewDesc& desc)
	{
		m_base->CreateDepthStencilView(
			descriptor,
			*texture.GetBaseTexture(),
			desc);
	}

	void GfxDevice::CreateShaderResourceView(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		GfxTexture& texture,
		const GfxShaderResourceViewDesc& desc)
	{
		if(!descriptorHeap.IsValid())
		{
			SI_WARNING(0, "descriptor heap is invalid.\n");
			return;
		}

		m_base->CreateShaderResourceView(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			*texture.GetBaseTexture(),
			desc);
	}

	void GfxDevice::CreateShaderResourceView(
		GfxDescriptor& descriptor,
		GfxTexture& texture,
		const GfxShaderResourceViewDesc& desc)
	{
		m_base->CreateShaderResourceView(
			descriptor,
			*texture.GetBaseTexture(),
			desc);
	}

	void GfxDevice::CreateShaderResourceView(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		GfxBuffer& buffer,
		const GfxShaderResourceViewDesc& desc)
	{
		if(!descriptorHeap.IsValid())
		{
			SI_WARNING(0, "descriptor heap is invalid.\n");
			return;
		}

		m_base->CreateShaderResourceView(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			*buffer.GetBaseBuffer(),
			desc);
	}

	void GfxDevice::CreateShaderResourceView(
		GfxDescriptor& descriptor,
		GfxBuffer& buffer,
		const GfxShaderResourceViewDesc& desc)
	{
		m_base->CreateShaderResourceView(
			descriptor,
			*buffer.GetBaseBuffer(),
			desc);
	}

	void GfxDevice::CreateUnorderedAccessView(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		GfxTexture& texture,
		const GfxUnorderedAccessViewDesc& desc)
	{
		if(!descriptorHeap.IsValid())
		{
			SI_WARNING(0, "descriptor heap is invalid.\n");
			return;
		}

		m_base->CreateUnorderedAccessView(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			*texture.GetBaseTexture(),
			desc);
	}

	void GfxDevice::CreateUnorderedAccessView(
		GfxDescriptor& descriptor,
		GfxTexture& texture,
		const GfxUnorderedAccessViewDesc& desc)
	{
		m_base->CreateUnorderedAccessView(
			descriptor,
			*texture.GetBaseTexture(),
			desc);
	}
	
	void GfxDevice::CreateSampler(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		const GfxSamplerDesc& desc)
	{
		if(!descriptorHeap.IsValid())
		{
			SI_WARNING(0, "descriptor heap is invalid.\n");
			return;
		}

		m_base->CreateSampler(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			desc);
	}
	
	void GfxDevice::CreateSampler(
		GfxDescriptor& descriptor,
		const GfxSamplerDesc& desc)
	{
		m_base->CreateSampler(
			descriptor,
			desc);
	}
	
	void GfxDevice::CreateConstantBufferView(
		GfxDescriptorHeap& descriptorHeap,
		uint32_t descriptorIndex,
		const GfxConstantBufferViewDesc& desc)
	{
		m_base->CreateConstantBufferView(
			*descriptorHeap.GetBaseDescriptorHeap(),
			descriptorIndex,
			desc);
	}

	void GfxDevice::CreateConstantBufferView(
		GfxDescriptor& descriptor,
		const GfxConstantBufferViewDesc& desc)
	{
		m_base->CreateConstantBufferView(
			descriptor,
			desc);
	}
	
	void GfxDevice::CopyDescriptors(
		uint32_t                 dstDescriptorRangeCount,
		const GfxCpuDescriptor*  dstDescriptorRangeStarts,
		const uint32_t*          dstDescriptorRangeSizes,
		uint32_t                 srcDescriptorRangeCount,
		const GfxCpuDescriptor*  srcDescriptorRangeStarts,
		const uint32_t*          srcDescriptorRangeSizes,
		GfxDescriptorHeapType    type)
	{
		m_base->CopyDescriptors(
			dstDescriptorRangeCount,
			dstDescriptorRangeStarts,
			dstDescriptorRangeSizes,
			srcDescriptorRangeCount,
			srcDescriptorRangeStarts,
			srcDescriptorRangeSizes,
			type);
	}

	void GfxDevice::CopyDescriptorsSimple(
		uint32_t                 descriptorCount,
		GfxCpuDescriptor         dstDescriptorRangeStart,
		GfxCpuDescriptor         srcDescriptorRangeStart,
		GfxDescriptorHeapType    type)
	{
		m_base->CopyDescriptorsSimple(
			descriptorCount,
			dstDescriptorRangeStart,
			srcDescriptorRangeStart,
			type);
	}

	// upload用のバッファを作って登録、Flushまで転送はしない.
	int GfxDevice::UploadBufferLater(
		GfxBuffer&              targetBuffer,
		const void*             srcBuffer,
		size_t                  srcBufferSize,
		GfxResourceStates       before,
		GfxResourceStates       after)
	{
		return m_base->UploadBufferLater(
			*targetBuffer.GetBaseBuffer(),
			srcBuffer,
			srcBufferSize,
			before,
			after);
	}

	int GfxDevice::UploadTextureLater(
		GfxTexture&             targetTexture,
		const void*             srcBuffer,
		size_t                  srcBufferSize,
		GfxResourceStates       before,
		GfxResourceStates       after)
	{
		return m_base->UploadTextureLater(
			*targetTexture.GetBaseTexture(),
			srcBuffer,
			srcBufferSize,
			before,
			after);
	}

	// upload用のバッファを転送する.
	int GfxDevice::FlushUploadPool(GfxGraphicsCommandList& commandList)
	{
		return m_base->FlushUploadPool(*commandList.GetBaseGraphicsCommandList());
	}
	
	PoolAllocatorEx* GfxDevice::GetObjectAllocator()
	{
		return m_base->GetObjectAllocator();
	}

	PoolAllocatorEx* GfxDevice::GetTempAllocator()
	{
		return m_base->GetTempAllocator();
	}


	void* GfxDevice::GetNative()
	{
		return m_base->GetNative();
	}
}
