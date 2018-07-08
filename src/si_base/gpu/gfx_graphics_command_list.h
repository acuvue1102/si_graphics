#pragma once

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_command_list.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_color.h"

namespace SI
{
	class BaseCommandList;
	class BaseGraphicsCommandList;
	class GfxTexture;
	class GfxGraphicsState;
	class GfxViewport;
	class GfxScissor;
	class GfxIndexBufferView;
	class GfxVertexBufferView;
	class GfxRootSignature;
	class GfxDevice;
	class GfxGpuResource;

	class GfxGraphicsCommandList : public GfxCommandList
	{
	public:
		GfxGraphicsCommandList(BaseGraphicsCommandList* base = nullptr);
		virtual ~GfxGraphicsCommandList();
		
		void ClearRenderTarget(const GfxCpuDescriptor& tex, const GfxColorRGBA& clearColor);
		void ClearRenderTarget(const GfxCpuDescriptor& tex, float r, float g, float b, float a)
		{
			ClearRenderTarget(tex, GfxColorRGBA(r,g,b,a));
		}
		void ClearDepthTarget(const GfxCpuDescriptor& tex, float depth);
		void ClearStencilTarget(const GfxCpuDescriptor& tex, uint8_t stencil);
		void ClearDepthStencilTarget(const GfxCpuDescriptor& tex, float depth, uint8_t stencil);
		
		int Reset(GfxGraphicsState* graphicsState);
		
		void ResourceBarrier(
			GfxTexture& texture,
			GfxResourceStates before,
			GfxResourceStates after,
			GfxResourceBarrierFlag flag = GfxResourceBarrierFlag::None);
		
		void ResourceBarrier(
			GfxGpuResource& resource,
			GfxResourceStates before,
			GfxResourceStates after,
			GfxResourceBarrierFlag flag = GfxResourceBarrierFlag::None);

		int Close();

		void SetGraphicsState(GfxGraphicsState& graphicsState);

		void SetGraphicsRootSignature(GfxRootSignature& rootSignature);

		void SetDescriptorHeaps(uint32_t descriptorHeapCount, GfxDescriptorHeap* const* descriptorHeaps);

		void SetGraphicsDescriptorTable(uint32_t tableIndex, GfxGpuDescriptor descriptor);
		
		void SetViewports(uint32_t count, const GfxViewport* viewPorts);
		void SetScissors(uint32_t count, const GfxScissor* scissors);

		void SetRenderTargets(
			uint32_t                 renderTargetCount,
			const GfxCpuDescriptor*  renderTargets,
			const GfxCpuDescriptor&  depthStencilTarget = GfxCpuDescriptor());
		
		void SetPrimitiveTopology(GfxPrimitiveTopology topology);
		
		void SetIndexBuffer(GfxIndexBufferView* indexBufferView);

		void SetVertexBuffers(uint32_t inputSlot, uint32_t viewCount, GfxVertexBufferView** bufferViews);
		
		void DrawIndexedInstanced(
			uint32_t indexCountPerInstance,
			uint32_t instanceCount         = 1,
			uint32_t startIndexLocation    = 0,
			uint32_t baseVertexLocation    = 0,
			uint32_t startInstanceLocation = 0);

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
