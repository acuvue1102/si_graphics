
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/gfx_texture.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_graphics_command_list.h"

namespace SI
{
	GfxGraphicsCommandList::GfxGraphicsCommandList(BaseGraphicsCommandList* base)
		: m_base(base)
	{
	}

	GfxGraphicsCommandList::~GfxGraphicsCommandList()
	{
	}

	void GfxGraphicsCommandList::ClearRenderTarget(GfxTexture& tex, float r, float g, float b, float a)
	{
		m_base->ClearRenderTarget(*tex.GetBaseTexture(), r, g, b, a);
	}

	int GfxGraphicsCommandList::Reset(GfxGraphicsState& graphicsState)
	{
		return m_base->Reset(*graphicsState.GetBaseGraphicsState());
	}
	
	void GfxGraphicsCommandList::ResourceBarrier(
		GfxTexture& texture,
		const GfxResourceState& before,
		const GfxResourceState& after)
	{
		m_base->ResourceBarrier(
			*texture.GetBaseTexture(),
			before,
			after);
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
		
	void GfxGraphicsCommandList::SetViewports(uint32_t count, GfxViewport* viewPorts)
	{
		m_base->SetViewports(count, viewPorts);
	}
		
	void GfxGraphicsCommandList::SetScissors(uint32_t count, GfxScissor* scissors)
	{
		m_base->SetScissors(count, scissors);
	}

	void GfxGraphicsCommandList::SetRenderTargets(
		uint32_t     renderTargetCount,
		GfxTexture*  renderTargets,
		GfxTexture*  depthStencilTarget)
	{
		m_base->SetRenderTargets(renderTargetCount, renderTargets, depthStencilTarget);
	}
		
	void GfxGraphicsCommandList::SetPrimitiveTopology(GfxPrimitiveTopology topology)
	{
		m_base->SetPrimitiveTopology(topology);
	}
		
	void GfxGraphicsCommandList::SetVertexBuffers(
		uint32_t              inputSlot,
		uint32_t              viewCount,
		GfxVertexBufferView*  bufferViews)
	{
		m_base->SetVertexBuffers(inputSlot, viewCount, bufferViews);
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

	BaseCommandList* GfxGraphicsCommandList::GetBaseCommandList()
	{
		return m_base;
	}

} // namespace SI
