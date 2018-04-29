#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>

namespace SI
{
	struct GfxBufferDesc;

	class BaseBuffer
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseBuffer();
		~BaseBuffer();
		
		int Initialize(ID3D12Device& device, const GfxBufferDesc& desc);
		
		void* Map  (uint32_t subResourceId);
		void  Unmap(uint32_t subResourceId);
				
		size_t                    GetSize()     const{ return m_bufferSizeInByte; }
		D3D12_GPU_VIRTUAL_ADDRESS GetLocation() const{ return m_location; };

	public:
		ComPtr<ID3D12Resource>& GetComPtrResource()
		{
			return m_resource;
		}

	private:
		ComPtr<ID3D12Resource>    m_resource;
		size_t                    m_bufferSizeInByte;
		D3D12_GPU_VIRTUAL_ADDRESS m_location;
	};

} // namespace SI

#endif // SI_USE_DX12
