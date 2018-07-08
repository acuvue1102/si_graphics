
#include "si_base/gpu\gfx_config.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"

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
		ID3D12Device& d3dDevice = *device.GetComPtrDevice().Get();
		ID3D12Resource& d3dResource = *targetBuffer.GetComPtrResource().Get();
		D3D12_RESOURCE_DESC resourceDesc = d3dResource.GetDesc();
						
		UINT subresourceNum = CalcSubresouceNum(
			resourceDesc.MipLevels,
			resourceDesc.DepthOrArraySize,
			resourceDesc.Dimension);
		SI_ASSERT(subresourceNum == 1);
			
		// 一時メモリアロケータからバッファを確保するstd::vector.
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>   layouts(subresourceNum);

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
		
	int BaseGraphicsCommandList::UploadTexture(
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
			
		// 一時メモリアロケータからバッファを確保するstd::vector.
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>   layouts(subresourceNum);
		GfxTempVector<UINT>                                 numRows(subresourceNum);
		GfxTempVector<UINT64>                               rowSizeInBytes(subresourceNum);

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

		GfxTempVector<D3D12_SUBRESOURCE_DATA> sourcesData(subresourceNum);
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

} // namespace SI

#endif // SI_USE_DX12
