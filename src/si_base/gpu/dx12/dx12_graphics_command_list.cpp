
#include "si_base/gpu\gfx_config.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/core/basic_function.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"
#include "si_base/gpu/gfx_utility.h"

namespace SI
{
	BaseGraphicsCommandList::BaseGraphicsCommandList()
		: m_uploadHeapArrayIndex(0)
		, m_currentRootSignature(nullptr)
	{
	}

	
	BaseGraphicsCommandList::~BaseGraphicsCommandList()
	{
		Terminate();
	}
	
	int BaseGraphicsCommandList::Initialize(ID3D12Device& device)
	{
		HRESULT hr = device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandAllocator", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCommandList));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandList", _com_error(hr).ErrorMessage());
			return -1;
		}
		m_graphicsCommandList->SetName(L"GfxGraphicsCommandList");

		hr = m_graphicsCommandList->Close();
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_graphicsCommandList->Close", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

	int BaseGraphicsCommandList::Terminate()
	{
		for(auto heap : m_uploadHeapArray)
		{
			heap.clear();
		}
		m_graphicsCommandList.Reset();
		m_commandAllocator.Reset();

		return 0;
	}
	
	int BaseGraphicsCommandList::UploadBuffer(
		BaseDevice& device,
		BaseBuffer& targetBuffer,
		const void* srcBuffer,
		size_t srcBufferSize)
	{	
		ComPtr<ID3D12Resource> bufferUploadHeap;
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
		if(device.CreateUploadBuffer(
			bufferUploadHeap,
			layouts,
			targetBuffer,
			srcBuffer,
			srcBufferSize) != 0)
		{
			return -1;
		}

		return UploadBuffer(targetBuffer, bufferUploadHeap, layouts);
	}

	int BaseGraphicsCommandList::UploadBuffer(
		BaseBuffer& targetBuffer,
		ComPtr<ID3D12Resource>& bufferUploadHeap,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts)
	{
		ID3D12Resource& d3dResource = *targetBuffer.GetComPtrResource().Get();
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
		
	int BaseGraphicsCommandList::UploadTexture(
		BaseDevice& device,
		BaseTexture& targetTexture,
		const void* srcBuffer,
		size_t srcBufferSize)
	{
		ComPtr<ID3D12Resource>                            textureUploadHeap;
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
		if(device.CreateUploadTexture(
			textureUploadHeap,
			layouts,
			targetTexture,
			srcBuffer,
			srcBufferSize) != 0)
		{
			return -1;
		}

		return UploadTexture(targetTexture, textureUploadHeap, layouts);
	}
		
	int BaseGraphicsCommandList::UploadTexture(
		BaseTexture& targetTexture,
		ComPtr<ID3D12Resource>& textureUploadHeap,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts)
	{
		ID3D12Resource& d3dResource = *targetTexture.GetComPtrResource().Get();
		UINT subresourceNum = (UINT)layouts.size();
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
	
} // namespace SI

#endif // SI_USE_DX12
