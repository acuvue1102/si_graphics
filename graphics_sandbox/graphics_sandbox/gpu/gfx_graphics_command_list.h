#pragma once

#include "gpu/gfx_config.h"
#include "gpu/gfx_command_list.h"
#include "gpu/gfx_enum.h"

namespace SI
{
	class BaseCommandList;
	class BaseGraphicsCommandList;
	class GfxTexture;
	class GfxGraphicsState;
	class GfxResourceState;
	class GfxViewport;
	class GfxScissor;
	class GfxVertexBufferView;
	class GfxRootSignature;

	class GfxGraphicsCommandList : public GfxCommandList
	{
	public:
		GfxGraphicsCommandList(BaseGraphicsCommandList* base = nullptr);
		virtual ~GfxGraphicsCommandList();

		void ClearRenderTarget(GfxTexture& tex, float r, float g, float b, float a);
		
		int Reset(GfxGraphicsState& graphicsState);
		
		void ResourceBarrier(
			GfxTexture& texture,
			const GfxResourceState& before,
			const GfxResourceState& after);

		int Close();

		void SetGraphicsState(GfxGraphicsState& graphicsState);

		void SetGraphicsRootSignature(GfxRootSignature& rootSignature);
		
		void SetViewports(uint32_t count, GfxViewport* viewPorts);
		
		void SetScissors(uint32_t count, GfxScissor* scissors);

		void SetRenderTargets(
			uint32_t     enderTargetCount,
			GfxTexture*  renderTargets,
			GfxTexture*  depthStencilTarget = nullptr);
		
		void SetPrimitiveTopology(GfxPrimitiveTopology topology);
		
		void SetVertexBuffers(uint32_t inputSlot, uint32_t viewCount, GfxVertexBufferView* bufferViews);

		void DrawInstanced(
			uint32_t vertexCountPerInstance,
			uint32_t instanceCount           = 1,
			uint32_t startVertexLocation     = 0,
			uint32_t startInstanceLocation   = 0);

	public:
		BaseGraphicsCommandList* GetBaseGraphicsCommandList(){ return m_base; }
		const BaseGraphicsCommandList* GetBaseGraphicsCommandList() const{ return m_base; }
		BaseCommandList* GetBaseCommandList() override;

	private:
		BaseGraphicsCommandList* m_base;
	};

} // namespace SI
