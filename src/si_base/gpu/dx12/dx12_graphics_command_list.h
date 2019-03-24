#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <comdef.h>
#include <vector>
#include <d3dx12.h>
#include "si_base/gpu/dx12/dx12_utility.h"
#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_command_list.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_compute_state.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_utility.h"
#include "si_base/gpu/gfx_texture.h"
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/gfx_viewport.h"
#include "si_base/gpu/gfx_gpu_resource.h"
#include "si_base/gpu/gfx_device_std_allocator.h"
#include "si_base/gpu/gfx_descriptor_heap.h"

namespace SI
{
	class BaseTexture;
	class BaseGraphicsState;

	class BaseGraphicsCommandList : public BaseCommandList
	{
	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseGraphicsCommandList();
		virtual ~BaseGraphicsCommandList();

		int Initialize(ID3D12Device& device);
		int Terminate();
		
		inline int Reset(BaseGraphicsState* graphicsState)
		{
			ID3D12PipelineState* pipelineState = graphicsState? graphicsState->GetComPtrGraphicsState().Get() : nullptr;

			HRESULT hr = m_commandAllocator->Reset();
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_commandAllocator->Reset", _com_error(hr).ErrorMessage());
				return -1;
			}

			hr = m_graphicsCommandList->Reset(m_commandAllocator.Get(), pipelineState);
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_graphicsCommandList->Reset", _com_error(hr).ErrorMessage());
				return -1;
			}

			m_currentRootSignature = nullptr;

			return 0;
		}

		inline int Close()
		{
			HRESULT hr = m_graphicsCommandList->Close();
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_graphicsCommandList->Close: %s", _com_error(hr).ErrorMessage());
				return -1;
			}

			return 0;
		}
		
		void SetPipelineState(BaseGraphicsState& graphicsState)
		{
			m_graphicsCommandList->SetPipelineState(graphicsState.GetComPtrGraphicsState().Get());
		}
		
		void SetPipelineState(BaseComputeState& computeState)
		{
			m_graphicsCommandList->SetPipelineState(computeState.GetComPtrComputeState().Get());
		}

		inline void ClearRenderTarget(const GfxCpuDescriptor& tex, const float* clearColor)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {};
			rtvHandle.ptr = tex.m_ptr;

			m_graphicsCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}

		inline void ClearDepthTarget(
			const GfxCpuDescriptor& tex,
			float clearDepth)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
			dsvHandle.ptr = tex.m_ptr;

			m_graphicsCommandList->ClearDepthStencilView(
				dsvHandle,
				D3D12_CLEAR_FLAG_DEPTH,
				clearDepth,
				0,
				0, nullptr);
		}

		inline void ClearStencilTarget(
			const GfxCpuDescriptor& tex,
			uint8_t stencil)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
			dsvHandle.ptr = tex.m_ptr;

			m_graphicsCommandList->ClearDepthStencilView(
				dsvHandle,
				D3D12_CLEAR_FLAG_STENCIL,
				1.0f,
				stencil,
				0, nullptr);
		}

		inline void ClearDepthStencilTarget(
			const GfxCpuDescriptor& tex,
			float clearDepth,
			uint8_t stencil)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
			dsvHandle.ptr = tex.m_ptr;

			m_graphicsCommandList->ClearDepthStencilView(
				dsvHandle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
				clearDepth,
				stencil,
				0, nullptr);
		}

		inline void ResourceBarrier(
			void* resource,
			GfxResourceStates before,
			GfxResourceStates after,
			GfxResourceBarrierFlag flag)
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = GetDx12ResourceBarrierFlag(flag);
			barrier.Transition.pResource   = (ID3D12Resource*)resource;
			barrier.Transition.StateBefore = GetDx12ResourceStates(before);
			barrier.Transition.StateAfter  = GetDx12ResourceStates(after);
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		
			m_graphicsCommandList->ResourceBarrier(1, &barrier);
		}
		
		inline bool SetGraphicsRootSignature(BaseRootSignature& rootSignature)
		{
			if(&rootSignature == m_currentRootSignature)
			{
				return false;
			}

			m_graphicsCommandList->SetGraphicsRootSignature(rootSignature.GetComPtrRootSignature().Get());

			m_currentRootSignature = &rootSignature;
			return true;
		}
		
		inline bool SetComputeRootSignature(BaseRootSignature& rootSignature)
		{
			if(&rootSignature == m_currentRootSignature)
			{
				return false;
			}

			m_graphicsCommandList->SetComputeRootSignature(rootSignature.GetComPtrRootSignature().Get());

			m_currentRootSignature = &rootSignature;
			return true;
		}

		inline void SetDescriptorHeaps(
			uint32_t descriptorHeapCount,
			GfxDescriptorHeap* const* descriptorHeaps)
		{
			SI_ASSERT(descriptorHeapCount <= 2);
			
			ID3D12DescriptorHeap* heaps[2] = {};
			uint32_t count = Min(descriptorHeapCount, 2u);
			for(uint32_t i=0; i<count; ++i)
			{
				heaps[i] = descriptorHeaps[i]->GetBaseDescriptorHeap()->GetDx12DescriptorHeap();
			}
			m_graphicsCommandList->SetDescriptorHeaps(descriptorHeapCount, heaps);
		}

		inline void SetGraphicsDescriptorTable(
			uint32_t rootIndex,
			GfxGpuDescriptor descriptor)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = descriptor.m_ptr;
			m_graphicsCommandList->SetGraphicsRootDescriptorTable(rootIndex, handle);
		}

		inline void SetGraphicsRootCBV(
			uint32_t rootIndex,
			GpuAddres gpuAddr)
		{
			m_graphicsCommandList->SetGraphicsRootConstantBufferView(rootIndex, (D3D12_GPU_VIRTUAL_ADDRESS)gpuAddr);
		}

		inline void SetGraphicsRootCBV(
			uint32_t rootIndex,
			const BaseBuffer& buffer)
		{
			D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = buffer.GetGpuAddr();
			m_graphicsCommandList->SetGraphicsRootConstantBufferView(rootIndex, gpuAddr);
		}

		inline void SetComputeDescriptorTable(
			uint32_t tableIndex,
			GfxGpuDescriptor descriptor)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = descriptor.m_ptr;
			m_graphicsCommandList->SetComputeRootDescriptorTable(tableIndex, handle);
		}
		
		inline void SetViewports(uint32_t count, const GfxViewport* viewPorts)
		{
			D3D12_VIEWPORT d3dViewports[8];
			SI_ASSERT(count <=ArraySize(d3dViewports));
			uint32_t viewPortCount = Min(count, (uint32_t)ArraySize(d3dViewports));

			for(uint32_t v=0; v<viewPortCount; ++v)
			{
				D3D12_VIEWPORT&     outV = d3dViewports[v];
				const GfxViewport&  inV  = viewPorts[v];

				outV.TopLeftX = inV.GetTopLeftX();
				outV.TopLeftY = inV.GetTopLeftY();
				outV.Width    = inV.GetWidth();
				outV.Height   = inV.GetHeight();
				outV.MinDepth = inV.GetMinDepth();
				outV.MaxDepth = inV.GetMaxDepth();
			}
			m_graphicsCommandList->RSSetViewports(viewPortCount, d3dViewports);
		}
		
		inline void SetScissors(uint32_t count, const GfxScissor* scissors)
		{
			D3D12_RECT d3dScissors[8];
			SI_ASSERT(count <=ArraySize(d3dScissors));
			uint32_t scissorCount = Min(count, (uint32_t)ArraySize(d3dScissors));

			for(uint32_t s=0; s<scissorCount; ++s)
			{
				D3D12_RECT&         outS = d3dScissors[s];
				const GfxScissor&   inS  = scissors[s];
				
				outS.left    = inS.GetLeft();
				outS.top     = inS.GetTop();
				outS.right   = inS.GetRight();
				outS.bottom  = inS.GetBottom();
			}
			m_graphicsCommandList->RSSetScissorRects(scissorCount, d3dScissors);
		}

		inline void SetRenderTargets(
			uint32_t                  renderTargetCount,
			const GfxCpuDescriptor*   renderTargets,
			const GfxCpuDescriptor&   depthStencilTarget)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handles[8];
			SI_ASSERT(renderTargetCount <= (uint32_t)ArraySize(handles));
			uint32_t handleCount = Min(renderTargetCount, (uint32_t)ArraySize(handles));

			for(uint32_t h=0; h<handleCount; ++h)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE&  outH = handles[h];
				const GfxCpuDescriptor&        inH = renderTargets[h];

				outH.ptr = inH.m_ptr;
			}
			
			if(depthStencilTarget.m_ptr)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle;
				depthStencilHandle.ptr = depthStencilTarget.m_ptr;
				m_graphicsCommandList->OMSetRenderTargets(handleCount, handles, FALSE, &depthStencilHandle);
			}
			else
			{
				m_graphicsCommandList->OMSetRenderTargets(handleCount, handles, FALSE, nullptr);
			}
		}
		
		inline void SetPrimitiveTopology(GfxPrimitiveTopology topology)
		{
			m_graphicsCommandList->IASetPrimitiveTopology(GetDx12PrimitiveTopology(topology));
		}

		inline void SetIndexBuffer(const GfxIndexBufferView* indexBufferView)
		{
			if(indexBufferView == nullptr)
			{
				m_graphicsCommandList->IASetIndexBuffer(nullptr);
				return;
			}

			const BaseBuffer* buffer = indexBufferView->GetBuffer().GetBaseBuffer();

			D3D12_INDEX_BUFFER_VIEW d3View;
			d3View.BufferLocation = buffer->GetLocation() + indexBufferView->GetOffset();
			d3View.Format         = GetDx12Format(indexBufferView->GetFormat());
			d3View.SizeInBytes    = (UINT)indexBufferView->GetSize();

			m_graphicsCommandList->IASetIndexBuffer(&d3View);
		}
		
		inline void SetVertexBuffers(uint32_t inputSlot, uint32_t viewCount, const GfxVertexBufferView* bufferViews)
		{
			if(bufferViews == nullptr)
			{
				m_graphicsCommandList->IASetVertexBuffers(inputSlot, viewCount, nullptr);
				return;
			}

			D3D12_VERTEX_BUFFER_VIEW d3Views[8];
			SI_ASSERT(viewCount <= (uint32_t)ArraySize(d3Views));
			uint32_t d3dViewCount = Min(viewCount, (uint32_t)ArraySize(d3Views));

			for(uint32_t v=0; v<d3dViewCount; ++v)
			{
				D3D12_VERTEX_BUFFER_VIEW&   outV = d3Views[v];
				const GfxVertexBufferView&  inV = bufferViews[v];
				
				const BaseBuffer* baseBuffer = inV.GetBuffer().GetBaseBuffer();

				outV.BufferLocation = baseBuffer->GetLocation() + inV.GetOffset();
				outV.SizeInBytes    = (uint32_t)inV.GetSize();
				outV.StrideInBytes  = (uint32_t)inV.GetStride();
			}

			m_graphicsCommandList->IASetVertexBuffers(inputSlot, d3dViewCount, d3Views);
		}

		inline void Dispatch(
			uint32_t threadGroupCountX,
			uint32_t threadGroupCountY,
			uint32_t threadGroupCountZ)
		{
			m_graphicsCommandList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
		}

		inline void DrawIndexedInstanced(
			uint32_t indexCountPerInstance,
			uint32_t instanceCount,
			uint32_t startIndexLocation,
			uint32_t baseVertexLocation,
			uint32_t startInstanceLocation)
		{
			m_graphicsCommandList->DrawIndexedInstanced(
				indexCountPerInstance,
				instanceCount,
				startIndexLocation,
				baseVertexLocation,
				startInstanceLocation);
		}

		inline void DrawInstanced(
			uint32_t vertexCountPerInstance,
			uint32_t instanceCount,
			uint32_t startVertexLocation,
			uint32_t startInstanceLocation)
		{
			m_graphicsCommandList->DrawInstanced(
				vertexCountPerInstance,
				instanceCount,
				startVertexLocation,
				startInstanceLocation);
		}
		
		int UploadBuffer(
			BaseDevice& device,
			BaseBuffer& targetBuffer,
			const void* srcBuffer,
			size_t srcBufferSize,
			GfxResourceStates before,
			GfxResourceStates after);
		
		int UploadBuffer(
			BaseBuffer& targetBuffer,
			ComPtr<ID3D12Resource>& bufferUploadHeap,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts,
			GfxResourceStates before,
			GfxResourceStates after);
		
		int UploadTexture(
			BaseDevice& device,
			BaseTexture& targetTexture,
			const void* srcBuffer,
			size_t srcBufferSize);
		
		int UploadTexture(
			BaseTexture& targetTexture,
			ComPtr<ID3D12Resource>& textureUploadHeap,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts);

	public:
		void OnExecute() override
		{
			m_uploadHeapArrayIndex = (++m_uploadHeapArrayIndex) % (ArraySize(m_uploadHeapArray));
			m_uploadHeapArray[m_uploadHeapArrayIndex].clear(); // 前に積んだuploadHeapを開放する.
		}

	public:
		ID3D12GraphicsCommandList* GetGraphicsCommandList()
		{
			return m_graphicsCommandList.Get();
		}

		ID3D12CommandList* GetCommandList() override
		{
			return m_graphicsCommandList.Get();
		}

	private:
		ComPtr<ID3D12CommandAllocator>    m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCommandList;

		// upload用のResourceの生存期間を管理.
		std::vector<ComPtr<ID3D12Resource>> m_uploadHeapArray[3];
		uint32_t                            m_uploadHeapArrayIndex;
		BaseRootSignature*                  m_currentRootSignature;
	};
} // namespace SI

#endif // SI_USE_DX12
