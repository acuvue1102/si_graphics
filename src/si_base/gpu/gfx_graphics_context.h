#pragma once

#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_command_list.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_linear_allocator.h"
#include "si_base/gpu/gfx_graphics_command_list.h"
#include "si_base/core/non_copyable.h"
#include "si_base/gpu/gfx_texture_ex.h"
#include "si_base/gpu/gfx_dynamic_descriptor_heap.h"

namespace SI
{
	class GfxTextureEx;
	class GfxBufferEx;
	class GfxDynamicSampler;
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
	class GfxBufferEx_Index;
	class GfxBufferEx_Vertex;

	class GfxGraphicsContext : private NonCopyable
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
			GfxGpuResource& resource,
			GfxResourceStates after,
			GfxResourceBarrierFlag flag = GfxResourceBarrierFlag::None);

		void SetGraphicsRootSignature(GfxRootSignatureEx& rootSignature);
		void SetComputeRootSignature(GfxRootSignatureEx& rootSignature);

		void SetDescriptorHeapsDirectly(uint32_t descriptorHeapCount, GfxDescriptorHeap* const* descriptorHeaps);
		
		void SetDescriptorHeaps(GfxDescriptorHeap* cbvSrvUavDescriptorHeap, GfxDescriptorHeap* samplerDescriptorHeap);
		void SetCbvSrvUavDescriptorHeap(GfxDescriptorHeap* cbvSrvUavDescriptorHeap);
		void SetSamplerDescriptorHeap(GfxDescriptorHeap* samplerDescriptorHeap);

		void SetGraphicsDescriptorTable(uint32_t rootIndex, GfxGpuDescriptor descriptor);
		
		void SetGraphicsRootCBV(uint32_t rootIndex, GpuAddres gpuAddr);
		void SetGraphicsRootCBV(uint32_t rootIndex, const GfxBuffer& buffer);
		
		void SetDynamicViewDescriptor( 
			uint32_t rootIndex, uint32_t offset,
			const GfxTextureEx& texture);
		void SetDynamicViewDescriptor( 
			uint32_t rootIndex, uint32_t offset,
			const GfxBufferEx& buffer);
		void SetDynamicViewDescriptor( 
			uint32_t rootIndex, uint32_t offset,
			GfxCpuDescriptor descriptor);
		void SetDynamicViewDescriptors(
			uint32_t rootIndex, uint32_t offset,
			uint32_t descriptorCount, const GfxCpuDescriptor* descriptors);
		
		void SetDynamicSamplerDescriptor(
			uint32_t rootIndex, uint32_t offset,
			const GfxDynamicSampler& sampler);
		void SetDynamicSamplerDescriptor(
			uint32_t rootIndex, uint32_t offset,
			GfxCpuDescriptor descriptor);
		void SetDynamicSamplerDescriptors(
			uint32_t rootIndex, uint32_t offset,
			uint32_t descriptorCount, const GfxCpuDescriptor* descriptors);
		
		void SetPipelineState(GfxGraphicsState& graphicsState);
		void SetPipelineState(GfxComputeState& computeState);
		
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
		void SetRenderTarget(
			const GfxTextureEx&  renderTarget)
		{
			GfxCpuDescriptor descriptor = renderTarget.GetRtvDescriptor().GetCpuDescriptor();
			SetRenderTargets(1, &descriptor);
		}
		void SetRenderTarget(
			const GfxTextureEx&  renderTarget,
			const GfxTextureEx&  depthStencilTarget)
		{
			GfxCpuDescriptor descriptor = renderTarget.GetRtvDescriptor().GetCpuDescriptor();
			GfxCpuDescriptor depthDescriptor = depthStencilTarget.GetDsvDescriptor().GetCpuDescriptor();
			SetRenderTargets(1, &descriptor, depthDescriptor);
		}
		
		void SetPrimitiveTopology(GfxPrimitiveTopology topology);
		
		void SetIndexBuffer(const GfxIndexBufferView* indexBufferView);
		void SetIndexBuffer(const GfxBufferEx_Index& indexBuffer);
		void SetVertexBuffers(uint32_t slot, uint32_t viewCount, const GfxVertexBufferView* bufferViews);
		void SetVertexBuffers(uint32_t slot, uint32_t viewCount, const GfxBufferEx_Vertex* const* buffers);
		void SetVertexBuffer (uint32_t slot, const GfxVertexBufferView& bufferViews);
		void SetVertexBuffer (uint32_t slot, const GfxBufferEx_Vertex& buffer);
		void SetDynamicVB    (uint32_t slot, size_t vertexCount, size_t stride, const void* data);
		void SetDynamicIB16(size_t indexCount, const uint16_t* data);
		void SetDynamicIB32(size_t indexCount, const uint32_t* data);
		
		void Dispatch(
		uint32_t threadGroupCountX,
		uint32_t threadGroupCountY=1,
		uint32_t threadGroupCountZ=1);
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
			size_t srcBufferSize,
			GfxResourceStates before = GfxResourceState::CopyDest,
			GfxResourceStates after = GfxResourceState::PixelShaderResource);

		int UploadBuffer(
			GfxDevice& device,
			GfxBufferEx& targetBuffer,
			const void* srcBuffer,
			size_t srcBufferSize,
			GfxResourceStates after);

		int UploadTexture(
			GfxDevice& device,
			GfxTexture& targetTexture,
			const void* srcBuffer,
			size_t srcBufferSize,
			GfxResourceStates before,
			GfxResourceStates after);
		int UploadTexture(
			GfxDevice& device,
			GfxTextureEx& targetTexture,
			const void* srcBuffer,
			size_t srcBufferSize,
			GfxResourceStates after);

	public:
		GfxGraphicsCommandList* GetGraphicsCommandList(){ return &m_commandList; }
		const GfxGraphicsCommandList* GetGraphicsCommandList() const{ return &m_commandList; }

		BaseGraphicsCommandList* GetBaseGraphicsCommandList(){ return m_base; }
		const BaseGraphicsCommandList* GetBaseGraphicsCommandList() const{ return m_base; }

	public:
		GfxResourceStates GetPenddingResourceState(uint32_t resourceStateHandle) const;
		void SetPenddingResourceState(uint32_t resourceStateHandle, GfxResourceStates states);

		GfxResourceStates GetCurrentResourceState(uint32_t resourceStateHandle) const;
		void SetCurrentResourceState(uint32_t resourceStateHandle, GfxResourceStates states);
		
	private:
		void BindDescriptorHeaps();
		
	private:
		GfxGraphicsCommandList   m_commandList;
		BaseGraphicsCommandList* m_base;
		GfxResourceStates*       m_penddingStates; // keep the first state for this context
		GfxResourceStates*       m_currentStates;
		uint32_t                 m_maxStateCount;
		GfxLinearAllocator       m_cpuLinearAllocator;
		GfxLinearAllocator       m_gpuLinearAllocator;
		
		GfxDynamicDescriptorHeap m_viewDynamicDescriptorHeap;
		GfxDynamicDescriptorHeap m_samplerDynamicDescriptorHeap;

		GfxDescriptorHeap*       m_cbvSrvUavDescriptorHeap; 
		GfxDescriptorHeap*       m_samplerDescriptorHeap;
	};

} // namespace SI
