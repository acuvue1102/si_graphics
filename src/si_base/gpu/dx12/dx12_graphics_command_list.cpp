
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
#include "si_base/gpu/dx12/dx12_raytracing_geometry.h"

namespace SI
{
	namespace
	{
		inline void AllocateUAVBuffer(
			ID3D12Device* pDevice,
			UINT64 bufferSize,
			ID3D12Resource **ppResource,
			D3D12_RESOURCE_STATES initialResourceState,
			const wchar_t* resourceName = nullptr)
		{
			D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
			uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			uploadHeapProperties.CreationNodeMask = 1;
			uploadHeapProperties.VisibleNodeMask = 1;

			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Alignment = 0;
			bufferDesc.Width = bufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			HRESULT hr = pDevice->CreateCommittedResource(
				&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				initialResourceState,
				nullptr,
				IID_PPV_ARGS(ppResource));

			SI_ASSERT(SUCCEEDED(hr));

			if (resourceName)
			{
				(*ppResource)->SetName(resourceName);
			}
		}

		inline void AllocateUploadBuffer(
			ID3D12Device* pDevice,
			void *pData,
			UINT64 datasize,
			ID3D12Resource **ppResource,
			const wchar_t* resourceName = nullptr)
		{
			D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
			uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			uploadHeapProperties.CreationNodeMask = 1;
			uploadHeapProperties.VisibleNodeMask = 1;

			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Alignment = 0;
			bufferDesc.Width = datasize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			HRESULT hr = pDevice->CreateCommittedResource(
				&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(ppResource));

			SI_ASSERT(SUCCEEDED(hr));

			if (resourceName)
			{
				(*ppResource)->SetName(resourceName);
			}
			void *pMappedData;
			(*ppResource)->Map(0, nullptr, &pMappedData);
			memcpy(pMappedData, pData, datasize);
			(*ppResource)->Unmap(0, nullptr);
		}
	}

	BaseGraphicsCommandList::BaseGraphicsCommandList()
		: m_uploadHeapArrayIndex(0)
		, m_currentRootSignature(nullptr)
		, m_buildingScene(nullptr)
	{
	}

	
	BaseGraphicsCommandList::~BaseGraphicsCommandList()
	{
		Terminate();
	}
	
	int BaseGraphicsCommandList::Initialize(BaseDevice& baseDevice)
	{
		ID3D12Device& device = *baseDevice.GetComPtrDevice().Get();

		HRESULT hr = device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandAllocator: %s", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCommandList));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandList: %s", _com_error(hr).ErrorMessage());
			return -1;
		}
		m_graphicsCommandList->SetName(L"GfxGraphicsCommandList");

		hr = m_graphicsCommandList->Close();
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_graphicsCommandList->Close: %s", _com_error(hr).ErrorMessage());
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
		size_t srcBufferSize,
		GfxResourceStates before,
		GfxResourceStates after)
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

		return UploadBuffer(targetBuffer, bufferUploadHeap, layouts, before, after);
	}

	int BaseGraphicsCommandList::UploadBuffer(
		BaseBuffer& targetBuffer,
		ComPtr<ID3D12Resource>& bufferUploadHeap,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts,
		GfxResourceStates before,
		GfxResourceStates after)
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
		barrier.Transition.StateBefore = GetDx12ResourceStates(before);
		barrier.Transition.StateAfter  = GetDx12ResourceStates(after);
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
		size_t srcBufferSize,
		GfxResourceStates before,
		GfxResourceStates after)
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

		return UploadTexture(targetTexture, textureUploadHeap, layouts, before, after);
	}
		
	int BaseGraphicsCommandList::UploadTexture(
		BaseTexture& targetTexture,
		ComPtr<ID3D12Resource>& textureUploadHeap,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts,
		GfxResourceStates before,
		GfxResourceStates after)
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
		barrier.Transition.StateBefore = GetDx12ResourceStates(before);
		barrier.Transition.StateAfter  = GetDx12ResourceStates(after);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_graphicsCommandList->ResourceBarrier(1, &barrier);
			
		// uploadが完了するまでuploadHeapは消せないので
		// 生存管理用のキューにためる.
		m_uploadHeapArray[m_uploadHeapArrayIndex].push_back(textureUploadHeap);

		return 0;
	}


	void BaseGraphicsCommandList::BeginBuildAccelerationStructures(BaseRaytracingScene& scene)
	{
		SI_ASSERT(m_buildingScene == nullptr);
		m_buildingScene = &scene;
	}

	void BaseGraphicsCommandList::AddGeometry(const GfxRaytracingGeometryDesc& geometryDesc)
	{
		SI_ASSERT(m_buildingScene);
		BaseDevice* baseDevice = BaseDevice::GetInstance();
		ID3D12Device5& device = *baseDevice->GetComPtrDevice().Get();

		D3D12_RAYTRACING_GEOMETRY_DESC dx12GeomDesc = {};

		dx12GeomDesc.Type  =  GetDx12RaytracingGeometryType(geometryDesc.m_type);
		if(dx12GeomDesc.Type==D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES)
		{
			D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& triangleDesc = dx12GeomDesc.Triangles;
			triangleDesc.IndexBuffer                = geometryDesc.m_triangle.m_indexBuffer;
			triangleDesc.IndexCount                 = geometryDesc.m_triangle.m_indexCount;
			triangleDesc.IndexFormat                = GetDx12Format(geometryDesc.m_triangle.m_indexFormat);
			triangleDesc.VertexBuffer.StartAddress  = geometryDesc.m_triangle.m_vertexBuffer;
			triangleDesc.VertexBuffer.StrideInBytes = geometryDesc.m_triangle.m_vertexStride;
			triangleDesc.VertexCount                = geometryDesc.m_triangle.m_vertexCount;
			triangleDesc.VertexFormat               = GetDx12Format(geometryDesc.m_triangle.m_vertexFormat);
		}
		else
		{
			SI_ASSERT(dx12GeomDesc.Type==D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS);

			D3D12_RAYTRACING_GEOMETRY_AABBS_DESC& aabbsDesc = dx12GeomDesc.AABBs;
			aabbsDesc.AABBCount = geometryDesc.m_aabbs.m_aabbCount;
			aabbsDesc.AABBs.StartAddress = geometryDesc.m_aabbs.m_startAddress;
			aabbsDesc.AABBs.StrideInBytes = geometryDesc.m_aabbs.m_strideInBytes;
		}
		dx12GeomDesc.Flags =  GetDx12RaytracingGeometryFlags(geometryDesc.m_flags);

		// Get required sizes for an acceleration structure.
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
		topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		topLevelInputs.Flags = buildFlags;
		topLevelInputs.NumDescs = 1;
		topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
		device.GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
		SI_ASSERT(0 < topLevelPrebuildInfo.ResultDataMaxSizeInBytes);

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = topLevelInputs;
		bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		bottomLevelInputs.pGeometryDescs = &dx12GeomDesc;

		device.GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
		
		SI_ASSERT(0 < bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes);

		AllocateUAVBuffer(
			&device,
			max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes),
			&m_buildingScene->m_scratchResource,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			L"ScratchResource");

		m_buildingScene->Initialize(
			baseDevice,
			bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes,
			topLevelPrebuildInfo.ResultDataMaxSizeInBytes);

		{
			D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
			instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
			instanceDesc.InstanceMask = 1;
			instanceDesc.AccelerationStructure = m_buildingScene->m_bottomLevelAccelerationStructure->GetGpuAddress();
			AllocateUploadBuffer(
				&device,
				&instanceDesc,
				sizeof(instanceDesc),
				&m_buildingScene->m_instanceDescs,
				L"InstanceDescs");
		}

		// Bottom Level Acceleration Structure desc
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
		bottomLevelBuildDesc.Inputs = bottomLevelInputs;
		bottomLevelBuildDesc.ScratchAccelerationStructureData = m_buildingScene->m_scratchResource->GetGPUVirtualAddress();
		bottomLevelBuildDesc.DestAccelerationStructureData = m_buildingScene->m_bottomLevelAccelerationStructure->GetGpuAddress();

		// Top Level Acceleration Structure desc
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
		topLevelInputs.InstanceDescs = m_buildingScene->m_instanceDescs->GetGPUVirtualAddress();
		topLevelBuildDesc.Inputs = topLevelInputs;
		topLevelBuildDesc.DestAccelerationStructureData = m_buildingScene->m_topLevelAccelerationStructure->GetGpuAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = m_buildingScene->m_scratchResource->GetGPUVirtualAddress();

		{
			m_graphicsCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);

			D3D12_RESOURCE_BARRIER uavBarrier = {};
			uavBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			uavBarrier.UAV.pResource = m_buildingScene->m_bottomLevelAccelerationStructure->GetComPtrResource().Get();
			m_graphicsCommandList->ResourceBarrier(1, &uavBarrier);
			m_graphicsCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
		}
	}

	void BaseGraphicsCommandList::EndBuildAccelerationStructures()
	{
		SI_ASSERT(m_buildingScene);
		m_buildingScene = nullptr;
	}
} // namespace SI

#endif // SI_USE_DX12
