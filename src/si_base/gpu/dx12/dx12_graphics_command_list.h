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
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/dx12/dx12_utility.h"
#include "si_base/gpu/gfx_texture.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/gfx_viewport.h"
#include "si_base/gpu/gfx_gpu_resource.h"

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
		
		void SetGraphicsState(BaseGraphicsState& graphicsState)
		{
			m_graphicsCommandList->SetPipelineState(graphicsState.GetComPtrGraphicsState().Get());
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
			uint32_t tableIndex,
			GfxGpuDescriptor descriptor)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = descriptor.m_ptr;
			m_graphicsCommandList->SetGraphicsRootDescriptorTable(tableIndex, handle);
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
			size_t srcBufferSize)
		{
			ID3D12Device& d3dDevice = *device.GetComPtrDevice().Get();
			ID3D12Resource& d3dResource = *targetBuffer.GetComPtrResource().Get();
			D3D12_RESOURCE_DESC resourceDesc = d3dResource.GetDesc();
						
			UINT subresourceNum = CalcSubresouceNum(
				resourceDesc.MipLevels,
				resourceDesc.DepthOrArraySize,
				resourceDesc.Dimension);
			SI_ASSERT(subresourceNum == 1);
			
			// リソースの各サイズを取得する.
			// メモリの動的確保どうにかしたい...
			std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>   layouts(subresourceNum);
			UINT64 totalBytes = 0;
			d3dDevice.GetCopyableFootprints(
				&resourceDesc,
				0, subresourceNum, 0,
				&layouts[0], nullptr, nullptr, &totalBytes);
			
			if(srcBufferSize < totalBytes)
			{
				SI_ASSERT(0);
				return -1;
			}

			// アップロード用のバッファを用意する.
			D3D12_HEAP_PROPERTIES heapProperties = {};
			heapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
			heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProperties.CreationNodeMask     = 1;
			heapProperties.VisibleNodeMask      = 1;
			
			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.MipLevels          = 1;
			bufferDesc.Format             = DXGI_FORMAT_UNKNOWN;
			bufferDesc.Width              = totalBytes;
			bufferDesc.Height             = 1;
			bufferDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;
			bufferDesc.DepthOrArraySize   = 1;
			bufferDesc.SampleDesc.Count   = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Alignment          = 0;
			
			ComPtr<ID3D12Resource> bufferUploadHeap;
			HRESULT hr = d3dDevice.CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&bufferUploadHeap));

			BYTE* pData = nullptr;
			hr = bufferUploadHeap->Map(0, NULL, reinterpret_cast<void**>(&pData));
			if (FAILED(hr))
			{
				SI_ASSERT(0, "error textureUploadHeap->Map", _com_error(hr).ErrorMessage());
				return -1;
			}
			memcpy(pData, srcBuffer, srcBufferSize);
			bufferUploadHeap->Unmap(0, NULL);
			
			m_graphicsCommandList->CopyBufferRegion(
				&d3dResource,
				0,
				bufferUploadHeap.Get(),
				layouts[0].Offset,
				layouts[0].Footprint.Width);

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = &d3dResource;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			m_graphicsCommandList->ResourceBarrier(1, &barrier);
			
			// uploadが完了するまでuploadHeapは消せないので
			// 生存管理用のキューにためる.
			m_uploadHeapArray[m_uploadHeapArrayIndex].push_back(bufferUploadHeap);

			return 0;
		}
		
		int UploadTexture(
			BaseDevice& device,
			BaseTexture& targetTexture,
			const void* srcBuffer,
			size_t srcBufferSize)
		{
			ID3D12Device& d3dDevice = *device.GetComPtrDevice().Get();
			ID3D12Resource& d3dResource = *targetTexture.GetComPtrResource().Get();
			D3D12_RESOURCE_DESC resourceDesc = d3dResource.GetDesc();

			UINT subresourceNum = CalcSubresouceNum(
				resourceDesc.MipLevels,
				resourceDesc.DepthOrArraySize,
				resourceDesc.Dimension);
			
			// リソースの各サイズを取得する.
			// メモリの動的確保どうにかしたい...
			std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>   layouts(subresourceNum);
			std::vector<UINT>                                 numRows(subresourceNum);
			std::vector<UINT64>                               rowSizeInBytes(subresourceNum);
			UINT64 totalBytes = 0;
			d3dDevice.GetCopyableFootprints(
				&resourceDesc,
				0, subresourceNum, 0,
				&layouts[0], &numRows[0], &rowSizeInBytes[0], &totalBytes);

			if(srcBufferSize < totalBytes)
			{
				SI_ASSERT(0);
				return -1;
			}

			// アップロード用のバッファを用意する.
			D3D12_HEAP_PROPERTIES heapProperties = {};
			heapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
			heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProperties.CreationNodeMask     = 1;
			heapProperties.VisibleNodeMask      = 1;
			
			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.MipLevels          = 1;
			bufferDesc.Format             = DXGI_FORMAT_UNKNOWN;
			bufferDesc.Width              = totalBytes;
			bufferDesc.Height             = 1;
			bufferDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;
			bufferDesc.DepthOrArraySize   = 1;
			bufferDesc.SampleDesc.Count   = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Alignment          = 0;
			
			ComPtr<ID3D12Resource> textureUploadHeap;
			HRESULT hr = d3dDevice.CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&textureUploadHeap));

			std::vector<D3D12_SUBRESOURCE_DATA> sourcesData(subresourceNum);
			size_t bpp = GetDx12FormatBits(resourceDesc.Format);
			const uint8_t* tmpSrc = (const uint8_t*)srcBuffer;
			for(UINT i=0; i<subresourceNum; ++i)
			{
				sourcesData[i].pData      = tmpSrc;
				sourcesData[i].RowPitch   = (layouts[i].Footprint.Width * bpp) / 8;
				sourcesData[i].SlicePitch = layouts[i].Footprint.Height * layouts[i].Footprint.Depth;

				tmpSrc += sourcesData[i].RowPitch * sourcesData[i].SlicePitch;
			}
			SI_ASSERT((uintptr_t)tmpSrc <= (uintptr_t)srcBuffer + srcBufferSize);

			BYTE* pData = nullptr;
			hr = textureUploadHeap->Map(0, NULL, reinterpret_cast<void**>(&pData));
			if (FAILED(hr))
			{
				SI_ASSERT(0, "error textureUploadHeap->Map", _com_error(hr).ErrorMessage());
				return -1;
			}
				
			for (UINT i=0; i<subresourceNum; ++i)
			{
				if (rowSizeInBytes[i] > (SIZE_T)-1)
				{
					SI_ASSERT(0);
					return -1;
				}

				D3D12_MEMCPY_DEST DestData =
				{
					pData + layouts[i].Offset,
					layouts[i].Footprint.RowPitch,
					layouts[i].Footprint.RowPitch * numRows[i]
				};

				MemcpySubresource(
					&DestData,
					&sourcesData[i],
					(SIZE_T)rowSizeInBytes[i],
					numRows[i],
					layouts[i].Footprint.Depth);
			}
			textureUploadHeap->Unmap(0, NULL);
			
			for (UINT i=0; i<subresourceNum; ++i)
			{
				D3D12_TEXTURE_COPY_LOCATION dst = {};
				dst.pResource        = &d3dResource;
				dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				dst.SubresourceIndex = i;
				
				D3D12_TEXTURE_COPY_LOCATION src = {};
				src.pResource        = textureUploadHeap.Get();
				src.Type             = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
				src.PlacedFootprint = layouts[i];

				m_graphicsCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
			}

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = &d3dResource;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			m_graphicsCommandList->ResourceBarrier(1, &barrier);
			
			// uploadが完了するまでuploadHeapは消せないので
			// 生存管理用のキューにためる.
			m_uploadHeapArray[m_uploadHeapArrayIndex].push_back(textureUploadHeap);

			return 0;
		}

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
