
#include "si_base/gpu/dx12/dx12_raytracing_shader_table.h"

#include "si_base/gpu/gfx_raytracing_state.h"
#include "si_base/gpu/gfx_raytracing_shader_table.h"
#include "si_base/gpu/dx12/dx12_raytracing_state.h"
#include "si_base/core/basic_function.h"

namespace SI
{
	namespace
	{
		size_t GetRecordSize(const GfxShaderRecord& record)
		{
			return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES +record.GetLocalRootArgumentSize();
		}

		size_t GetTableSize(const GfxShaderTableDesc& table)
		{
			size_t size = 0;
			uint32_t recordCount = table.GetShaderRecordCount();
			for(uint32_t i=0; i<recordCount; ++i)
			{
				size += GetRecordSize(table.GetShaderRecord(i));
			}

			return size;
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> AllocateUpdateBuffer(
			ID3D12Device& device,
			size_t bufferSize)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;

			D3D12_HEAP_PROPERTIES properties = {};
			properties.Type = D3D12_HEAP_TYPE_UPLOAD;
			properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			properties.CreationNodeMask = 1;
			properties.VisibleNodeMask = 1;

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
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			HRESULT hr = device.CreateCommittedResource(
				&properties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&resource));
			SI_ASSERT(SUCCEEDED(hr));

			return resource;
		}

		BaseRaytracingShaderTable CreateShaderTable(
			ID3D12Device& device,
			const GfxShaderTableDesc& tableDesc)
		{
			size_t tableSize = GetTableSize(tableDesc);
			SI_ASSERT(tableSize!=0);

			uint32_t recordCount = tableDesc.GetShaderRecordCount();

			size_t maxLocalArgumentSize = tableDesc.GetMaxLocalRootArgumentSize();
			size_t shaderRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + maxLocalArgumentSize;
			shaderRecordSize = SI::AlignUp(shaderRecordSize, (size_t)D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
			size_t bufferSize = recordCount * shaderRecordSize;

			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			resource = AllocateUpdateBuffer(device, bufferSize);

			D3D12_RANGE range = {0, 0};
			uint8_t* mappedData = nullptr;
			HRESULT hr = resource->Map(0, &range, (void**)&mappedData);
			SI_ASSERT(SUCCEEDED(hr));

			for(uint32_t i=0; i<recordCount; ++i)
			{
				const GfxShaderRecord& record = tableDesc.GetShaderRecord(i);
				uint8_t* recordBuffer = mappedData;

				memcpy(mappedData, record.GetShaderId(), D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
				if(record.GetLocalRootArgument())
				{
					memcpy(mappedData+D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, record.GetLocalRootArgument(), record.GetLocalRootArgumentSize());
				}

				mappedData += shaderRecordSize;
			}

			BaseRaytracingShaderTable table;
			table.m_buffer = resource;
			table.m_gpuAddress = resource->GetGPUVirtualAddress();
			table.m_size = bufferSize;
			table.m_stride = shaderRecordSize;

			return table;
		}
	}

	BaseRaytracingShaderTables::BaseRaytracingShaderTables()
	{
	}

	BaseRaytracingShaderTables::~BaseRaytracingShaderTables()
	{
	}

	int BaseRaytracingShaderTables::Initialize(
		ID3D12Device& device,
		const GfxRaytracingShaderTablesDesc& desc)
	{
		m_rayGenShaderTable   = CreateShaderTable(device, desc.GetRayGenTableDesc());
		m_missShaderTable     = CreateShaderTable(device, desc.GetMissTableDesc());
		m_hitGroupShaderTable = CreateShaderTable(device, desc.GetHitGroupTableDesc());

		return 0;
	}

} // namespace SI
