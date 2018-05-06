
#include <comdef.h>
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/dx12/dx12_buffer.h"

#if SI_USE_DX12
namespace SI
{
	BaseBuffer::BaseBuffer()
		: m_bufferSizeInByte(0)
		, m_location(0)
	{
	}

	BaseBuffer::~BaseBuffer()
	{
	}

	int BaseBuffer::Initialize(ID3D12Device& device, const GfxBufferDesc& desc)
	{
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = GetDx12HeapType(desc.m_heapType);
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = desc.m_bufferSizeInByte;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
		if(desc.m_heapType == kGfxHeapType_Default)
		{
			resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		HRESULT hr = device.CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			resourceState,
			nullptr,
			IID_PPV_ARGS(&m_resource));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error device.CreateCommittedResource", _com_error(hr).ErrorMessage());
			return -1;
		}

		m_bufferSizeInByte = desc.m_bufferSizeInByte;
		m_location         = m_resource->GetGPUVirtualAddress();

		return 0;
	}
	
	void* BaseBuffer::Map(uint32_t subResourceId)
	{
		D3D12_RANGE range = {0, 0};
		void* addr = nullptr;
		HRESULT hr = m_resource->Map(subResourceId, &range, &addr);
		if(FAILED(hr))
		{
			SI_ASSERT(0);
			return nullptr;
		}

		return addr;
	}

	void  BaseBuffer::Unmap(uint32_t subResourceId)
	{
		m_resource->Unmap(subResourceId, nullptr);
	}

} // namespace SI

#endif // SI_USE_DX12
