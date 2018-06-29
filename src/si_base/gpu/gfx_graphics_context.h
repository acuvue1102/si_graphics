#pragma once

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_command_list.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_linear_allocator.h"

namespace SI
{
	class GfxTextureEx;
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

	class GfxGraphicsContext
	{
	public:
		GfxGraphicsContext();
		~GfxGraphicsContext();
		
		void Initialize();
		void Terminate();
		
		void Reset();

		void Close();

		void ClearRenderTarget(const GfxTextureEx& tex);
		void ClearDepthTarget(const GfxTextureEx& tex);
		void ClearStencilTarget(const GfxTextureEx& tex);
		void ClearDepthStencilTarget(const GfxTextureEx& tex);
				
		void ResourceBarrier(
			GfxTextureEx& texture,
			GfxResourceStates after,
			GfxResourceBarrierFlag flag = GfxResourceBarrierFlag::kNone);

		void SetGraphicsRootSignature(GfxRootSignature& rootSignature);

		void SetDescriptorHeaps(uint32_t descriptorHeapCount, GfxDescriptorHeap* const* descriptorHeaps);

		void SetGraphicsDescriptorTable(uint32_t tableIndex, GfxGpuDescriptor descriptor);

		void SetGraphicsPso(GfxGraphicsState& graphicsState);
		
		void SetViewports(uint32_t count, const GfxViewport* viewPorts);
		void SetViewport(const GfxViewport& viewPort){ SetViewports(1, &viewPort); }
		
		void SetScissors(uint32_t count, const GfxScissor* scissors);
		void SetScissor(const GfxScissor& scissor){ SetScissors(1, &scissor); }
		
		void SetRenderTargets(
			uint32_t                 renderTargetCount,
			const GfxCpuDescriptor*  renderTargets,
			const GfxCpuDescriptor&  depthStencilTarget = GfxCpuDescriptor());
		void SetRenderTarget(
			const GfxCpuDescriptor&  renderTarget,
			const GfxCpuDescriptor&  depthStencilTarget = GfxCpuDescriptor())
		{
			SetRenderTargets(1, &renderTarget, depthStencilTarget);
		}
		
		void SetPrimitiveTopology(GfxPrimitiveTopology topology);
		
		void SetIndexBuffer(GfxIndexBufferView* indexBufferView);
		void SetVertexBuffers(uint32_t slot, uint32_t viewCount, const GfxVertexBufferView* const* bufferViews);
		void SetVertexBuffer (uint32_t slot, const GfxVertexBufferView& bufferViews);
		void SetDynamicVB    (uint32_t slot, size_t vertexCount, size_t stride, const void* data);
		void SetDynamicIB16(size_t indexCount, const uint16_t* data);
		void SetDynamicIB32(size_t indexCount, const uint32_t* data);

		void Draw(
			uint32_t vertexCount,
			uint32_t baseVertexLocation    = 0);
		void DrawIndexed(
			uint32_t indexCount,
			uint32_t startIndexLocation    = 0,
			uint32_t baseVertexLocation    = 0);
		void DrawInstanced(
			uint32_t vertexCountPerInstance,
			uint32_t instanceCount           = 1,
			uint32_t startVertexLocation     = 0,
			uint32_t startInstanceLocation   = 0);
		void DrawIndexedInstanced(
			uint32_t indexCountPerInstance,
			uint32_t instanceCount         = 1,
			uint32_t startIndexLocation    = 0,
			uint32_t baseVertexLocation    = 0,
			uint32_t startInstanceLocation = 0);
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

	public:
		GfxResourceStates GetPenddingResourceState(uint32_t resourceStateHandle) const;
		void SetPenddingResourceState(uint32_t resourceStateHandle, GfxResourceStates states);

	private:
		GfxResourceStates GetCurrentResourceState(uint32_t resourceStateHandle) const;
		void SetCurrentResourceState(uint32_t resourceStateHandle, GfxResourceStates states);
		
	private:
		BaseGraphicsCommandList* m_base;
		GfxResourceStates*       m_penddingStates; // keep the first state for this context
		GfxResourceStates*       m_currentStates;
		uint32_t                 m_maxStateCount;
		GfxLinearAllocator       m_cpuLinearAllocator;
		GfxLinearAllocator       m_gpuLinearAllocator;
	};

} // namespace SI
