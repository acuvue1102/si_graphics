
#include "si_base/gpu/dx12/dx12_descriptor_heap.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/gpu/gfx_buffer.h"

namespace SI
{
	BaseDescriptorHeap::BaseDescriptorHeap()
	{
		m_cpuDescriptor.ptr = 0;
		m_gpuDescriptor.ptr = 0;
	}

	BaseDescriptorHeap::~BaseDescriptorHeap()
	{
		Terminate();
	}
	
	int BaseDescriptorHeap::Initialize(
		ID3D12Device& device,
		const GfxDescriptorHeapDesc& desc)
	{
		D3D12_DESCRIPTOR_HEAP_DESC kDesc =
		{
			GetDx12DescriptorHeapType(desc.m_type),
			desc.m_descriptorCount,
			GetDx12DescriptorHeapFlag(desc.m_flag),
			0,                                        //UINT NodeMask;
		};

		HRESULT hr = device.CreateDescriptorHeap(&kDesc, IID_PPV_ARGS(&m_descriptorHeap));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateDescriptorHeap", _com_error(hr).ErrorMessage());
			return -1;
		}
			
		m_cpuDescriptor = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_gpuDescriptor = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		return 0;
	}

	int BaseDescriptorHeap::Terminate()
	{
		m_descriptorHeap.Reset();
		m_cpuDescriptor.ptr = 0;
		m_gpuDescriptor.ptr = 0;
		return 0;
	}
	
	//void BaseDescriptorHeap::CreateRenderTargetView(
	//	ID3D12Device& device,
	//	uint32_t descriptorIndex,
	//	BaseTexture& texture,
	//	const GfxRenderTargetViewDesc& desc)
	//{
	//	size_t descriptorSize = BaseDevice::GetDescriptorSize(kGfxDescriptorHeapType_RTV);
	//	D3D12_CPU_DESCRIPTOR_HANDLE descriptor = GetDx12Descriptor(descriptorIndex, descriptorSize);
	//	device.CreateRenderTargetView(texture.GetComPtrResource().Get(), nullptr, descriptor);
	//}
	
	void BaseDescriptorHeap::CreateShaderResourceView(
		ID3D12Device& device,
		uint32_t descriptorIndex,
		BaseTexture& texture,
		const GfxShaderResourceViewDesc& desc)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format                  = GetDx12Format(desc.m_format);
		srvDesc.ViewDimension           = GetDx12SrvDimension(desc.m_srvDimension);
		switch(desc.m_srvDimension)
		{
		case kGfxDimension_Buffer:
		case kGfxDimension_Texture1D:
			srvDesc.Texture1D.MipLevels = desc.m_miplevels;
			break;
		case kGfxDimension_Texture1DArray:
			srvDesc.Texture1DArray.MipLevels = desc.m_miplevels;
			srvDesc.Texture1DArray.ArraySize = desc.m_arraySize;
			break;
		case kGfxDimension_Texture2D:
			srvDesc.Texture2D.MipLevels = desc.m_miplevels;
			break;
		case kGfxDimension_Texture2DArray:
			srvDesc.Texture2DArray.MipLevels = desc.m_miplevels;
			srvDesc.Texture2DArray.ArraySize = desc.m_arraySize;
			break;
		case kGfxDimension_Texture2DMS:
		case kGfxDimension_Texture2DMSArray:
			break;
		case kGfxDimension_Texture3D:
			srvDesc.Texture3D.MipLevels = desc.m_miplevels;
			break;
		case kGfxDimension_TextureCube:
			srvDesc.TextureCube.MipLevels = desc.m_miplevels;
			break;
		case kGfxDimension_TextureCubeArray:
			srvDesc.TextureCubeArray.MipLevels = desc.m_miplevels;
			srvDesc.TextureCubeArray.NumCubes  = desc.m_arraySize;
			break;
		default:
			SI_ASSERT(0);
			break;
		}

		size_t descriptorSize = BaseDevice::GetDescriptorSize(kGfxDescriptorHeapType_CbvSrvUav);
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor = GetDx12CpuDescriptor(descriptorIndex, descriptorSize);
		device.CreateShaderResourceView(texture.GetComPtrResource().Get(), &srvDesc, descriptor);
	}
	
	void BaseDescriptorHeap::CreateSampler(
		ID3D12Device& device,
		uint32_t descriptorIndex,
		const GfxSamplerDesc& desc)
	{
		D3D12_SAMPLER_DESC samplerDesc = {};		
		samplerDesc.Filter        = GetDx12Filter(desc.m_filter);
		samplerDesc.AddressU      = GetDx12TextureAddress(desc.m_addressU);
		samplerDesc.AddressV      = GetDx12TextureAddress(desc.m_addressV);
		samplerDesc.AddressW      = GetDx12TextureAddress(desc.m_addressW);
		samplerDesc.MipLODBias    = desc.m_mipLODBias;
		samplerDesc.MaxAnisotropy = desc.m_maxAnisotropy;
		samplerDesc.ComparisonFunc= GetDx12ComparisonFunc(desc.m_comparisonFunc);
		samplerDesc.BorderColor[0] = desc.m_borderColor[0];
		samplerDesc.BorderColor[1] = desc.m_borderColor[1];
		samplerDesc.BorderColor[2] = desc.m_borderColor[2];
		samplerDesc.BorderColor[3] = desc.m_borderColor[3];
		samplerDesc.MinLOD         = desc.m_minLOD;
		samplerDesc.MaxLOD         = desc.m_maxLOD;

		size_t descriptorSize = BaseDevice::GetDescriptorSize(kGfxDescriptorHeapType_Sampler);
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor = GetDx12CpuDescriptor(descriptorIndex, descriptorSize);
		device.CreateSampler(&samplerDesc, descriptor);
	}

	void BaseDescriptorHeap::CreateConstantBufferView(
		ID3D12Device& device,
		uint32_t descriptorIndex,
		const GfxConstantBufferViewDesc& desc)
	{
		BaseBuffer* baseBuffer = desc.m_buffer->GetBaseBuffer();

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantDesc = {};
		constantDesc.BufferLocation = baseBuffer->GetLocation();
		constantDesc.SizeInBytes    = baseBuffer->GetSize();

		size_t descriptorSize = BaseDevice::GetDescriptorSize(kGfxDescriptorHeapType_CbvSrvUav);
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor = GetDx12CpuDescriptor(descriptorIndex, descriptorSize);
		device.CreateConstantBufferView(&constantDesc, descriptor);
	}
	
	GfxCpuDescriptor BaseDescriptorHeap::GetCpuDescriptor(GfxDescriptorHeapType type, uint32_t descriptorIndex) const
	{
		size_t descriptorSize = BaseDevice::GetDescriptorSize(type);

		GfxCpuDescriptor d;
		d.m_ptr = m_cpuDescriptor.ptr + descriptorIndex * descriptorSize;
		return d;
	}
	
	GfxGpuDescriptor BaseDescriptorHeap::GetGpuDescriptor(GfxDescriptorHeapType type, uint32_t descriptorIndex) const
	{
		size_t descriptorSize = BaseDevice::GetDescriptorSize(type);

		GfxGpuDescriptor d;
		d.m_ptr = m_gpuDescriptor.ptr + descriptorIndex * descriptorSize;
		return d;
	}
	
	D3D12_CPU_DESCRIPTOR_HANDLE BaseDescriptorHeap::GetDx12CpuDescriptor(uint32_t descriptorId, size_t descriptorSize) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle(m_cpuDescriptor);
		handle.ptr += descriptorId * descriptorSize;
		return handle;
	}
} // namespace SI

#endif // SI_USE_DX12
