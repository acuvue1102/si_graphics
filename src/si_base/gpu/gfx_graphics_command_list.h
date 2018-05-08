#pragma once

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_command_list.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"

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
	class GfxDevice;

	class GfxGraphicsCommandList : public GfxCommandList
	{
	public:
		GfxGraphicsCommandList(BaseGraphicsCommandList* base = nullptr);
		virtual ~GfxGraphicsCommandList();

		void ClearRenderTarget(const GfxCpuDescriptor& tex, float r, float g, float b, float a);
		
		int Reset(GfxGraphicsState* graphicsState);
		
		void ResourceBarrier(
			GfxTexture& texture,
			const GfxResourceState& before,
			const GfxResourceState& after,
			GfxResourceBarrierFlag flag = kGfxResourceBarrierFlag_None);

		int Close();

		void SetGraphicsState(GfxGraphicsState& graphicsState);

		void SetGraphicsRootSignature(GfxRootSignature& rootSignature);

		void SetDescriptorHeaps(uint32_t descriptorHeapCount, GfxDescriptorHeap* const* descriptorHeaps);

		void SetGraphicsDescriptorTable(uint32_t tableIndex, GfxGpuDescriptor descriptor);
		
		void SetViewports(uint32_t count, GfxViewport* viewPorts);
		
		void SetScissors(uint32_t count, GfxScissor* scissors);

		void SetRenderTargets(
			uint32_t                 renderTargetCount,
			const GfxCpuDescriptor*  renderTargets,
			const GfxCpuDescriptor&  depthStencilTarget = GfxCpuDescriptor());
		
		void SetPrimitiveTopology(GfxPrimitiveTopology topology);
		
		void SetVertexBuffers(uint32_t inputSlot, uint32_t viewCount, GfxVertexBufferView* bufferViews);

		void DrawInstanced(
			uint32_t vertexCountPerInstance,
			uint32_t instanceCount           = 1,
			uint32_t startVertexLocation     = 0,
			uint32_t startInstanceLocation   = 0);

	public:
		int UploadBuffer(
			GfxDevice& device,
			GfxBuffer& targetBuffer,
			const void* srcBuffer,
			size_t srcBufferSize);

		int UploadTexture(
			GfxDevice& device,
			GfxTexture& targetTexture,
			const void* srcBuffer,
			size_t srcBufferSize);

	public:
		BaseGraphicsCommandList* GetBaseGraphicsCommandList(){ return m_base; }
		const BaseGraphicsCommandList* GetBaseGraphicsCommandList() const{ return m_base; }
		BaseCommandList* GetBaseCommandList() override;

	private:
		BaseGraphicsCommandList* m_base;
	};

} // namespace SI
