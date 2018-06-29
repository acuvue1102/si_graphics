
#include "si_base/gpu/gfx_graphics_context.h"

#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/gfx_texture_ex.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_core.h"

namespace SI
{
	GfxGraphicsContext::GfxGraphicsContext()
		: m_base(nullptr)
		, m_penddingStates(nullptr)
		, m_currentStates(nullptr)
		, m_maxStateCount(0)
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
		m_base = SI_BASE_DEVICE().CreateGraphicsCommandList();


		SI_ASSERT(m_maxStateCount==0 && m_penddingStates == nullptr && m_currentStates==nullptr);
		m_maxStateCount = SI_RESOURCE_STATES_POOL().GetMaxStateCount();
		m_penddingStates = SI_NEW_ARRAY(GfxResourceStates, m_maxStateCount);
		m_currentStates  = SI_NEW_ARRAY(GfxResourceStates, m_maxStateCount);
		m_cpuLinearAllocator.Initialize(true);
		m_gpuLinearAllocator.Initialize(false);
	}

	void GfxGraphicsContext::Terminate()
	{
		if(m_base)
		{
			m_gpuLinearAllocator.Terminate();
			m_cpuLinearAllocator.Terminate();

			m_maxStateCount = 0;
			SI_DELETE_ARRAY(m_penddingStates);
			SI_DELETE_ARRAY(m_currentStates);

			SI_BASE_DEVICE().ReleaseGraphicsCommandList(m_base);
			m_base = nullptr;
		}
	}
	
	void GfxGraphicsContext::Reset()
	{
		m_base->Reset(nullptr);

		for(uint32_t i=0; i<m_maxStateCount; ++i)
		{
			m_penddingStates[i] = GfxResourceState::kPendding;
			m_currentStates [i] = GfxResourceState::kPendding;
		}
		
		m_cpuLinearAllocator.Reset();
		m_gpuLinearAllocator.Reset();
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
		GfxCpuDescriptor descriptor = tex.GetRtvDescriptor().GetCpuDescriptor();

		GfxDepthStencil clearDepthStencil = tex.GetClearDepthStencil();
		m_base->ClearDepthStencilTarget(
			descriptor,
			clearDepthStencil.GetDepth(), clearDepthStencil.GetStencil());
	}
	
	void GfxGraphicsContext::ResourceBarrier(
		GfxTextureEx& texture,
		GfxResourceStates after,
		GfxResourceBarrierFlag flag)
	{
		GfxResourceStates before = GetCurrentResourceState(texture.GetResourceStateHandle());

		if(before == GfxResourceState::kPendding)
		{
			// 前のコンテキストでの設定の影響を受けるので、後で解決する.
			SetPenddingResourceState(texture.GetResourceStateHandle(), after);
			SetCurrentResourceState (texture.GetResourceStateHandle(), after);

			return;
		}

		m_base->ResourceBarrier(
			*texture.GetBaseTexture(),
			before,
			after,
			flag);
		
		SetCurrentResourceState(texture.GetResourceStateHandle(), after);
	}
	
	void GfxGraphicsContext::SetGraphicsPso(GfxGraphicsState& graphicsState)
	{
		m_base->SetGraphicsState(*graphicsState.GetBaseGraphicsState());
	}

	void GfxGraphicsContext::SetGraphicsRootSignature(GfxRootSignature& rootSignature)
	{
		m_base->SetGraphicsRootSignature(*rootSignature.GetBaseRootSignature());
	}

	void GfxGraphicsContext::SetDescriptorHeaps(
		uint32_t descriptorHeapCount,
		GfxDescriptorHeap* const* descriptorHeaps)
	{
		m_base->SetDescriptorHeaps(descriptorHeapCount, descriptorHeaps);
	}

	void GfxGraphicsContext::SetGraphicsDescriptorTable(
		uint32_t tableIndex,
		GfxGpuDescriptor descriptor)
	{
		m_base->SetGraphicsDescriptorTable(tableIndex, descriptor);
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
	
	void GfxGraphicsContext::SetIndexBuffer(GfxIndexBufferView* indexBufferView)
	{
		m_base->SetIndexBuffer(indexBufferView);
	}
		
	void GfxGraphicsContext::SetVertexBuffers(
		uint32_t                          slot,
		uint32_t                          viewCount,
		const GfxVertexBufferView* const* bufferViews)
	{
		m_base->SetVertexBuffers(slot, viewCount, bufferViews);
	}
	
	void GfxGraphicsContext::SetVertexBuffer(uint32_t slot, const GfxVertexBufferView& bufferViews)
	{
		const GfxVertexBufferView* views[1] = {&bufferViews};
		SetVertexBuffers(slot, 1, views);
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
			GfxFormat::kR16_Uint,
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
			GfxFormat::kR32_Uint,
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

	GfxResourceStates GfxGraphicsContext::GetPenddingResourceState(uint32_t resourceStateHandle) const
	{
		SI_ASSERT(resourceStateHandle < m_maxStateCount);
		return m_penddingStates[resourceStateHandle];
	}

	void GfxGraphicsContext::SetPenddingResourceState(uint32_t resourceStateHandle, GfxResourceStates states)
	{
		SI_ASSERT(resourceStateHandle < m_maxStateCount);
		SI_ASSERT(m_penddingStates[resourceStateHandle] == GfxResourceState::kPendding);
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

} // namespace SI
