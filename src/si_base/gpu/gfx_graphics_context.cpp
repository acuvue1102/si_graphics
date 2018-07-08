﻿
#include "si_base/gpu/gfx_graphics_context.h"

#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/gfx_texture_ex.h"
#include "si_base/gpu/gfx_buffer_ex.h"
#include "si_base/gpu/gfx_sampler_ex.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_core.h"

namespace SI
{
	GfxGraphicsContext::GfxGraphicsContext()
		: m_base(nullptr)
		, m_penddingStates(nullptr)
		, m_currentStates(nullptr)
		, m_maxStateCount(0)
		, m_cbvSrvUavDescriptorHeap(nullptr)
		, m_samplerDescriptorHeap(nullptr)
	{
	}

	GfxGraphicsContext::~GfxGraphicsContext()
	{
		SI_ASSERT(m_penddingStates==nullptr);
		SI_ASSERT(m_currentStates==nullptr);
	}
	
	void GfxGraphicsContext::Initialize()
	{
		SI_ASSERT(m_base==nullptr);
		m_commandList = SI_DEVICE().CreateGraphicsCommandList();
		m_base = m_commandList.GetBaseGraphicsCommandList();


		SI_ASSERT(m_maxStateCount==0 && m_penddingStates == nullptr && m_currentStates==nullptr);
		m_maxStateCount = SI_RESOURCE_STATES_POOL().GetMaxStateCount();
		m_penddingStates = SI_NEW_ARRAY(GfxResourceStates, m_maxStateCount);
		m_currentStates  = SI_NEW_ARRAY(GfxResourceStates, m_maxStateCount);
		for(uint32_t i=0; i<m_maxStateCount; ++i)
		{
			m_penddingStates[i] = GfxResourceState::Pendding;
			m_currentStates [i] = GfxResourceState::Pendding;
		}
		m_cpuLinearAllocator.Initialize(true);
		m_gpuLinearAllocator.Initialize(false);
		
		m_viewDynamicDescriptorHeap.Initialize(SI::GfxDescriptorHeapType::CbvSrvUav);
		m_samplerDynamicDescriptorHeap.Initialize(SI::GfxDescriptorHeapType::Sampler);
	}

	void GfxGraphicsContext::Terminate()
	{
		if(m_base)
		{
			m_samplerDynamicDescriptorHeap.Terminate();
			m_viewDynamicDescriptorHeap.Terminate();

			m_gpuLinearAllocator.Terminate();
			m_cpuLinearAllocator.Terminate();

			m_maxStateCount = 0;
			SI_DELETE_ARRAY(m_penddingStates);
			SI_DELETE_ARRAY(m_currentStates);

			SI_DEVICE().ReleaseGraphicsCommandList(m_commandList);
			m_base = nullptr;
		}
	}
	
	void GfxGraphicsContext::Reset()
	{
		m_base->Reset(nullptr);
		
		uint32_t maxAllocatedStateCount = SI_RESOURCE_STATES_POOL().GetMaxAllocatedStateCount(); // 確保された最大ハンドル.
		for(uint32_t i=0; i<maxAllocatedStateCount; ++i)
		{
			m_penddingStates[i] = GfxResourceState::Pendding;
			m_currentStates [i] = GfxResourceState::Pendding;
		}
		
		m_cpuLinearAllocator.Reset();
		m_gpuLinearAllocator.Reset();
		
		m_viewDynamicDescriptorHeap.Reset();
		m_samplerDynamicDescriptorHeap.Reset();
		
		m_cbvSrvUavDescriptorHeap = nullptr;
		m_samplerDescriptorHeap   = nullptr;
	}
	
	void GfxGraphicsContext::Close()
	{
		m_base->Close();
	}

	void GfxGraphicsContext::ClearRenderTarget(const GfxTextureEx& tex)
	{
		GfxCpuDescriptor descriptor = tex.GetRtvDescriptor().GetCpuDescriptor();
		GfxColorRGBA clearColor = tex.GetClearColor();

		m_base->ClearRenderTarget(descriptor, clearColor.GetPtr());
	}

	void GfxGraphicsContext::ClearDepthTarget(const GfxTextureEx& tex)
	{
		GfxCpuDescriptor descriptor = tex.GetRtvDescriptor().GetCpuDescriptor();

		GfxDepthStencil clearDepthStencil = tex.GetClearDepthStencil();
		m_base->ClearDepthTarget( descriptor, clearDepthStencil.GetDepth() );
	}

	void GfxGraphicsContext::ClearStencilTarget(const GfxTextureEx& tex)
	{
		GfxCpuDescriptor descriptor = tex.GetRtvDescriptor().GetCpuDescriptor();

		GfxDepthStencil clearDepthStencil = tex.GetClearDepthStencil();
		m_base->ClearStencilTarget( descriptor, clearDepthStencil.GetStencil() );
	}

	void GfxGraphicsContext::ClearDepthStencilTarget(const GfxTextureEx& tex)
	{
		GfxCpuDescriptor descriptor = tex.GetDsvDescriptor().GetCpuDescriptor();

		GfxDepthStencil clearDepthStencil = tex.GetClearDepthStencil();
		m_base->ClearDepthStencilTarget(
			descriptor,
			clearDepthStencil.GetDepth(), clearDepthStencil.GetStencil());
	}
	
	void GfxGraphicsContext::ResourceBarrier(
		GfxGpuResource& resource,
		GfxResourceStates after,
		GfxResourceBarrierFlag flag)
	{
		uint32_t resourceStateHandle = resource.GetResourceStateHandle();
		GfxResourceStates before = GetCurrentResourceState(resourceStateHandle);

		if(before == GfxResourceState::Pendding)
		{
			// 前のコンテキストでの設定の影響を受けるので、後で解決する.
			SetPenddingResourceState(resourceStateHandle, after);
			SetCurrentResourceState (resourceStateHandle, after);

			return;
		}

		if(before == after) return;

		m_base->ResourceBarrier(
			resource.GetNativeResource(),
			before,
			after,
			flag);
		
		SetCurrentResourceState(resourceStateHandle, after);
	}
	
	void GfxGraphicsContext::SetPipelineState(GfxGraphicsState& graphicsState)
	{
		m_base->SetGraphicsState(*graphicsState.GetBaseGraphicsState());
	}

	void GfxGraphicsContext::SetGraphicsRootSignature(GfxRootSignatureEx& rootSignature)
	{
		if(!m_base->SetGraphicsRootSignature(*rootSignature.GetRootSignature().GetBaseRootSignature()))
		{
			return; // 前と同じなので何もしない.
		}
		
		m_viewDynamicDescriptorHeap.ParseGraphicsRootSignature(rootSignature);
		m_samplerDynamicDescriptorHeap.ParseGraphicsRootSignature(rootSignature);
	}

	void GfxGraphicsContext::SetDescriptorHeapsDirectly(
		uint32_t descriptorHeapCount,
		GfxDescriptorHeap* const* descriptorHeaps)
	{
		m_base->SetDescriptorHeaps(descriptorHeapCount, descriptorHeaps);
	}
	
	void GfxGraphicsContext::SetDescriptorHeaps(
		GfxDescriptorHeap* cbvSrvUavDescriptorHeap,
		GfxDescriptorHeap* samplerDescriptorHeap)
	{
		m_cbvSrvUavDescriptorHeap = cbvSrvUavDescriptorHeap;
		m_samplerDescriptorHeap   = samplerDescriptorHeap;

		BindDescriptorHeaps();
	}

	void GfxGraphicsContext::SetCbvSrvUavDescriptorHeap(GfxDescriptorHeap* cbvSrvUavDescriptorHeap)
	{
		m_cbvSrvUavDescriptorHeap = cbvSrvUavDescriptorHeap;
		
		BindDescriptorHeaps();
	}

	void GfxGraphicsContext::SetSamplerDescriptorHeap(GfxDescriptorHeap* samplerDescriptorHeap)
	{
		m_samplerDescriptorHeap = samplerDescriptorHeap;
		
		BindDescriptorHeaps();
	}

	void GfxGraphicsContext::SetGraphicsDescriptorTable(
		uint32_t tableIndex,
		GfxGpuDescriptor descriptor)
	{
		m_base->SetGraphicsDescriptorTable(tableIndex, descriptor);
	}
	
	void GfxGraphicsContext::SetDynamicViewDescriptor( 
		uint32_t rootIndex, uint32_t offset,
		const GfxTextureEx& texture)
	{
		SetDynamicViewDescriptor(rootIndex, offset, texture.GetSrvDescriptor().GetCpuDescriptor());
	}

	void GfxGraphicsContext::SetDynamicViewDescriptor( 
		uint32_t rootIndex, uint32_t offset,
		const GfxBufferEx& buffer)
	{
		SetDynamicViewDescriptor(rootIndex, offset, buffer.GetSrvDescriptor().GetCpuDescriptor());
	}
	
	void GfxGraphicsContext::SetDynamicViewDescriptor(
		uint32_t rootIndex, uint32_t offset,
		GfxCpuDescriptor descriptor)
	{
		m_viewDynamicDescriptorHeap.SetGraphicsDescriptorHandles(
			rootIndex, offset,
			1, &descriptor);
	}
	
	void GfxGraphicsContext::SetDynamicViewDescriptors(
		uint32_t rootIndex, uint32_t offset,
		uint32_t descriptorCount, const GfxCpuDescriptor* descriptors)
	{
		m_viewDynamicDescriptorHeap.SetGraphicsDescriptorHandles(
			rootIndex, offset,
			descriptorCount, descriptors);
	}

	void GfxGraphicsContext::SetDynamicSamplerDescriptor(
		uint32_t rootIndex, uint32_t offset,
		const GfxSamplerEx& sampler)
	{
		SetDynamicSamplerDescriptor(
			rootIndex, offset,
			sampler.GetDescriptor().GetCpuDescriptor());
	}
	
	void GfxGraphicsContext::SetDynamicSamplerDescriptor(
		uint32_t rootIndex, uint32_t offset,
		GfxCpuDescriptor descriptor)
	{
		m_samplerDynamicDescriptorHeap.SetGraphicsDescriptorHandles(
			rootIndex, offset,
			1, &descriptor);
	}
	
	void GfxGraphicsContext::SetDynamicSamplerDescriptors(
		uint32_t rootIndex, uint32_t offset,
		uint32_t descriptorCount, const GfxCpuDescriptor* descriptors)
	{
		m_samplerDynamicDescriptorHeap.SetGraphicsDescriptorHandles(
			rootIndex, offset,
			descriptorCount, descriptors);
	}
		
	void GfxGraphicsContext::SetViewports(uint32_t count, const GfxViewport* viewPorts)
	{
		m_base->SetViewports(count, viewPorts);
	}
		
	void GfxGraphicsContext::SetScissors(uint32_t count, const GfxScissor* scissors)
	{
		m_base->SetScissors(count, scissors);
	}

	void GfxGraphicsContext::SetRenderTargets(
		uint32_t                 renderTargetCount,
		const GfxCpuDescriptor*  renderTargets,
		const GfxCpuDescriptor&  depthStencilTarget)
	{
		m_base->SetRenderTargets(renderTargetCount, renderTargets, depthStencilTarget);
	}
		
	void GfxGraphicsContext::SetPrimitiveTopology(GfxPrimitiveTopology topology)
	{
		m_base->SetPrimitiveTopology(topology);
	}
	
	void GfxGraphicsContext::SetIndexBuffer(const GfxIndexBufferView* indexBufferView)
	{
		m_base->SetIndexBuffer(indexBufferView);
	}
	
	void GfxGraphicsContext::SetIndexBuffer(const GfxBufferEx_Index& indexBuffer)
	{
		GfxIndexBufferView indexBufferView(
			indexBuffer.GetBuffer(), indexBuffer.GetFormat(), indexBuffer.GetSize());
		m_base->SetIndexBuffer(&indexBufferView);
	}
		
	void GfxGraphicsContext::SetVertexBuffers(
		uint32_t                          slot,
		uint32_t                          viewCount,
		const GfxVertexBufferView*        bufferViews)
	{
		m_base->SetVertexBuffers(slot, viewCount, bufferViews);
	}
		
	void GfxGraphicsContext::SetVertexBuffers(
		uint32_t                          slot,
		uint32_t                          viewCount,
		const GfxBufferEx_Vertex* const* buffers)
	{
		GfxVertexBufferView views[16];
		if((uint32_t)ArraySize(views) < viewCount)
		{
			SI_ASSERT(0);
			return;
		}
		for(uint32_t i=0; i<viewCount; ++i)
		{
			const GfxBufferEx_Vertex& buffer = *buffers[i];
			
			views[i] = GfxVertexBufferView(
				buffer.GetBuffer(),
				buffer.GetSize(),
				buffer.GetStride(),
				buffer.GetOffset() );
		}

		m_base->SetVertexBuffers(slot, viewCount, views);
	}
	
	void GfxGraphicsContext::SetVertexBuffer(uint32_t slot, const GfxVertexBufferView& bufferViews)
	{
		SetVertexBuffers(slot, 1, &bufferViews);
	}
	
	void GfxGraphicsContext::SetVertexBuffer(uint32_t slot, const GfxBufferEx_Vertex& buffer)
	{
		GfxVertexBufferView bufferView(
			buffer.GetBuffer(), buffer.GetSize(), buffer.GetStride(), buffer.GetOffset() );
		SetVertexBuffers(slot, 1, &bufferView);
	}

	void GfxGraphicsContext::SetDynamicVB(
		uint32_t slot, size_t vertexCount, size_t stride, const void* data)
	{
		size_t size = vertexCount * stride;
		GfxLinearAllocatorMemory mem = m_cpuLinearAllocator.Allocate(size);
		
		memcpy(mem.GetCpuAddr(), data, size);

		GfxVertexBufferView view;
		view.SetBuffer(mem.GetBuffer());
		view.SetSize(mem.GetSize());
		view.SetStride(size);
		view.SetOffset(mem.GetOffset());

		SetVertexBuffer(slot, view);
	}

	void GfxGraphicsContext::SetDynamicIB16(size_t indexCount, const uint16_t* data)
	{
		size_t size = indexCount * sizeof(uint16_t);
		GfxLinearAllocatorMemory mem = m_cpuLinearAllocator.Allocate(size);

		memcpy(mem.GetCpuAddr(), data, size);

		GfxIndexBufferView view(
			mem.GetBuffer(),
			GfxFormat::R16_Uint,
			mem.GetSize(),
			mem.GetOffset());

		SetIndexBuffer(&view);
	}

	void GfxGraphicsContext::SetDynamicIB32(size_t indexCount, const uint32_t* data)
	{
		size_t size = indexCount * sizeof(uint32_t);
		GfxLinearAllocatorMemory mem = m_cpuLinearAllocator.Allocate(size);

		memcpy(mem.GetCpuAddr(), data, size);

		GfxIndexBufferView view(
			mem.GetBuffer(),
			GfxFormat::R32_Uint,
			mem.GetSize(),
			mem.GetOffset());

		SetIndexBuffer(&view);
	}

	void GfxGraphicsContext::Draw(
		uint32_t vertexCount,
		uint32_t baseVertexLocation)
	{
		DrawInstanced(vertexCount, 1, baseVertexLocation, 0);
	}

	void GfxGraphicsContext::DrawIndexed(
		uint32_t indexCount,
		uint32_t startIndexLocation,
		uint32_t baseVertexLocation)
	{
		DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}

	void GfxGraphicsContext::DrawInstanced(
		uint32_t vertexCountPerInstance,
		uint32_t instanceCount,
		uint32_t startVertexLocation,
		uint32_t startInstanceLocation)
	{
		m_viewDynamicDescriptorHeap.CopyAndBindGraphicsTables(*this);
		m_samplerDynamicDescriptorHeap.CopyAndBindGraphicsTables(*this);

		m_base->DrawInstanced(
			vertexCountPerInstance,
			instanceCount,
			startVertexLocation,
			startInstanceLocation);
	}

	void GfxGraphicsContext::DrawIndexedInstanced(
		uint32_t indexCountPerInstance,
		uint32_t instanceCount,
		uint32_t startIndexLocation,
		uint32_t baseVertexLocation,
		uint32_t startInstanceLocation)
	{
		m_viewDynamicDescriptorHeap.CopyAndBindGraphicsTables(*this);
		m_samplerDynamicDescriptorHeap.CopyAndBindGraphicsTables(*this);

		m_base->DrawIndexedInstanced(
			indexCountPerInstance,
			instanceCount,
			startIndexLocation,
			baseVertexLocation,
			startInstanceLocation);
	}

	int GfxGraphicsContext::UploadBuffer(
		GfxDevice& device,
		GfxBuffer& targetBuffer,
		const void* srcBuffer,
		size_t srcBufferSize)
	{
		return m_base->UploadBuffer(
			*device.GetBaseDevice(),
			*targetBuffer.GetBaseBuffer(),
			srcBuffer,
			srcBufferSize);
	}

	int GfxGraphicsContext::UploadBuffer(
		GfxDevice& device,
		GfxBufferEx& targetBuffer,
		const void* srcBuffer,
		size_t srcBufferSize)
	{
		return m_base->UploadBuffer(
			*device.GetBaseDevice(),
			*targetBuffer.GetBaseBuffer(),
			srcBuffer,
			srcBufferSize);
	}

	int GfxGraphicsContext::UploadTexture(
		GfxDevice& device,
		GfxTexture& targetTexture,
		const void* srcBuffer,
		size_t srcBufferSize)
	{
		return m_base->UploadTexture(
			*device.GetBaseDevice(),
			*targetTexture.GetBaseTexture(),
			srcBuffer,
			srcBufferSize);
	}

	int GfxGraphicsContext::UploadTexture(
		GfxDevice& device,
		GfxTextureEx& targetTexture,
		const void* srcBuffer,
		size_t srcBufferSize)
	{
		return m_base->UploadTexture(
			*device.GetBaseDevice(),
			*targetTexture.GetBaseTexture(),
			srcBuffer,
			srcBufferSize);
	}

	GfxResourceStates GfxGraphicsContext::GetPenddingResourceState(uint32_t resourceStateHandle) const
	{
		SI_ASSERT(resourceStateHandle < m_maxStateCount);
		return m_penddingStates[resourceStateHandle];
	}

	void GfxGraphicsContext::SetPenddingResourceState(uint32_t resourceStateHandle, GfxResourceStates states)
	{
		SI_ASSERT(resourceStateHandle < m_maxStateCount);
		SI_ASSERT(m_penddingStates[resourceStateHandle] == GfxResourceState::Pendding);
		m_penddingStates[resourceStateHandle] = states;
	}
	
	GfxResourceStates GfxGraphicsContext::GetCurrentResourceState(uint32_t resourceStateHandle) const
	{
		SI_ASSERT(resourceStateHandle < m_maxStateCount);
		return m_currentStates[resourceStateHandle];
	}

	void GfxGraphicsContext::SetCurrentResourceState(uint32_t resourceStateHandle, GfxResourceStates states)
	{
		SI_ASSERT(resourceStateHandle < m_maxStateCount);
		m_currentStates[resourceStateHandle] = states;
	}

	void GfxGraphicsContext::BindDescriptorHeaps()
	{
		if(m_cbvSrvUavDescriptorHeap && m_samplerDescriptorHeap)
		{
			GfxDescriptorHeap* descriptorHeaps[] = {m_cbvSrvUavDescriptorHeap, m_samplerDescriptorHeap};
			m_base->SetDescriptorHeaps(2, descriptorHeaps);
		}
		else if(m_cbvSrvUavDescriptorHeap)
		{
			m_base->SetDescriptorHeaps(1, &m_cbvSrvUavDescriptorHeap);
		}
		else if(m_samplerDescriptorHeap)
		{
			m_base->SetDescriptorHeaps(1, &m_samplerDescriptorHeap);
		}
	}

} // namespace SI
