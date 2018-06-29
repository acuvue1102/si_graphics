
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/gfx_texture.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_graphics_command_list.h"
#include "si_base/gpu/gfx_device.h"

namespace SI
{
	GfxGraphicsCommandList::GfxGraphicsCommandList(BaseGraphicsCommandList* base)
		: m_base(base)
	{
	}

	GfxGraphicsCommandList::~GfxGraphicsCommandList()
	{
	}

	void GfxGraphicsCommandList::ClearRenderTarget(const GfxCpuDescriptor& tex, const GfxColorRGBA& clearColor)
	{
		m_base->ClearRenderTarget(tex, clearColor.GetPtr());
	}

	void GfxGraphicsCommandList::ClearDepthTarget(
		const GfxCpuDescriptor& tex,
		float depth)
	{
		m_base->ClearDepthTarget(tex, depth);
	}

	void GfxGraphicsCommandList::ClearStencilTarget(
		const GfxCpuDescriptor& tex,
		uint8_t stencil)
	{
		m_base->ClearStencilTarget(tex, stencil);
	}

	void GfxGraphicsCommandList::ClearDepthStencilTarget(
		const GfxCpuDescriptor& tex,
		float depth,
		uint8_t stencil)
	{
		m_base->ClearDepthStencilTarget(tex, depth, stencil);
	}

	int GfxGraphicsCommandList::Reset(GfxGraphicsState* graphicsState)
	{
		return m_base->Reset(graphicsState? graphicsState->GetBaseGraphicsState() : nullptr);
	}
	
	void GfxGraphicsCommandList::ResourceBarrier(
		GfxTexture& texture,
		GfxResourceStates before,
		GfxResourceStates after,
		GfxResourceBarrierFlag flag)
	{
		m_base->ResourceBarrier(
			*texture.GetBaseTexture(),
			before,
			after,
			flag);
	}
	
	int GfxGraphicsCommandList::Close()
	{
		return m_base->Close();
	}
	
	void GfxGraphicsCommandList::SetGraphicsState(GfxGraphicsState& graphicsState)
	{
		m_base->SetGraphicsState(*graphicsState.GetBaseGraphicsState());
	}

	void GfxGraphicsCommandList::SetGraphicsRootSignature(GfxRootSignature& rootSignature)
	{
		m_base->SetGraphicsRootSignature(*rootSignature.GetBaseRootSignature());
	}

	void GfxGraphicsCommandList::SetDescriptorHeaps(
		uint32_t descriptorHeapCount,
		GfxDescriptorHeap* const* descriptorHeaps)
	{
		m_base->SetDescriptorHeaps(descriptorHeapCount, descriptorHeaps);
	}

	void GfxGraphicsCommandList::SetGraphicsDescriptorTable(
		uint32_t tableIndex,
		GfxGpuDescriptor descriptor)
	{
		m_base->SetGraphicsDescriptorTable(tableIndex, descriptor);
	}
		
	void GfxGraphicsCommandList::SetViewports(uint32_t count, const GfxViewport* viewPorts)
	{
		m_base->SetViewports(count, viewPorts);
	}
		
	void GfxGraphicsCommandList::SetScissors(uint32_t count, const GfxScissor* scissors)
	{
		m_base->SetScissors(count, scissors);
	}

	void GfxGraphicsCommandList::SetRenderTargets(
		uint32_t                 renderTargetCount,
		const GfxCpuDescriptor*  renderTargets,
		const GfxCpuDescriptor&  depthStencilTarget)
	{
		m_base->SetRenderTargets(renderTargetCount, renderTargets, depthStencilTarget);
	}
		
	void GfxGraphicsCommandList::SetPrimitiveTopology(GfxPrimitiveTopology topology)
	{
		m_base->SetPrimitiveTopology(topology);
	}
	
	void GfxGraphicsCommandList::SetIndexBuffer(GfxIndexBufferView* indexBufferView)
	{
		m_base->SetIndexBuffer(indexBufferView);
	}
		
	void GfxGraphicsCommandList::SetVertexBuffers(
		uint32_t              inputSlot,
		uint32_t              viewCount,
		GfxVertexBufferView** bufferViews)
	{
		m_base->SetVertexBuffers(inputSlot, viewCount, bufferViews);
	}

	void GfxGraphicsCommandList::DrawIndexedInstanced(
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

	void GfxGraphicsCommandList::DrawInstanced(
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

	int GfxGraphicsCommandList::UploadBuffer(
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

	int GfxGraphicsCommandList::UploadTexture(
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

	BaseCommandList* GfxGraphicsCommandList::GetBaseCommandList()
	{
		return m_base;
	}

} // namespace SI
