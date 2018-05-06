
#include "si_base/gpu/dx12/dx12_root_signature.h"

#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/core/scope_exit.h"
#include "si_base/memory/pool_allocator.h"
#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_device.h"

#if SI_USE_DX12
namespace SI
{
	BaseRootSignature::BaseRootSignature()
	{
	}

	BaseRootSignature::~BaseRootSignature()
	{
		Terminate();
	}

	int BaseRootSignature::Initialize(BaseDevice& device, const GfxRootSignatureDesc& desc)
	{
		ID3D12Device& d3dDevice = *device.GetComPtrDevice().Get();

#if 0
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#endif
		
		PoolAllocatorEx* tempAllocator = device.GetTempAllocator();
		D3D12_ROOT_PARAMETER1* parameters = tempAllocator->NewArray<D3D12_ROOT_PARAMETER1>(desc.m_tableCount);
		for(uint32_t t=0; t<desc.m_tableCount; ++t)
		{
			D3D12_ROOT_PARAMETER1& outParameter = parameters[t];
			outParameter.ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			outParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			D3D12_ROOT_DESCRIPTOR_TABLE1& outTable = outParameter.DescriptorTable;
			const GfxDescriptorHeapTable& inTable = desc.m_tables[t];
			
			D3D12_DESCRIPTOR_RANGE1* ranges = tempAllocator->NewArray<D3D12_DESCRIPTOR_RANGE1>(desc.m_tableCount);

			for(uint32_t r=0; r<inTable.m_rangeCount; ++r)
			{
				D3D12_DESCRIPTOR_RANGE1&  outRange = ranges[r];
				const GfxDescriptorRange&  inRange = inTable.m_ranges[r];
				
				outRange.RangeType           = GetDx12DescriptorRangeType(inRange.m_rangeType);
				outRange.NumDescriptors      = inRange.m_descriptorCount;
				outRange.BaseShaderRegister  = inRange.m_shaderRegisterIndex;
				outRange.RegisterSpace       = 0;

				if(inRange.m_rangeType==kGfxDescriptorRangeType_Sampler)
				{
					outRange.Flags           = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				}
				else
				{
					outRange.Flags           = GetDx12DescriptorRangeFlags(inRange.m_rangeFlag);
				}
				outRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			}
			
			outTable.NumDescriptorRanges = inTable.m_rangeCount;
			outTable.pDescriptorRanges   = ranges;
		}

		// このスコープを抜けると全て開放するように...
		SI_SCOPE_EXIT(
			for(uint32_t i=0; i<desc.m_tableCount; ++i)
			{
				tempAllocator->DeleteArray(parameters[i].DescriptorTable.pDescriptorRanges);
			}
			tempAllocator->DeleteArray(parameters);
		);
		
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc1 = rootSignatureDesc.Desc_1_1;
		rootSignatureDesc1.NumParameters     = desc.m_tableCount;
		rootSignatureDesc1.pParameters       = parameters;
		rootSignatureDesc1.NumStaticSamplers = 0;//1;
		rootSignatureDesc1.pStaticSamplers   = nullptr;//&sampler;
		rootSignatureDesc1.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> errCode;
		HRESULT hr = D3D12SerializeVersionedRootSignature(
			&rootSignatureDesc,
			&signature,
			&errCode);
		if(FAILED(hr))
		{
			const char* errString = (const char*)errCode->GetBufferPointer();
			SI_ASSERT(0, "error D3D12SerializeRootSignature\n%s", errString);
			return -1;
		}

		hr = d3dDevice.CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateRootSignature", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

	int BaseRootSignature::Terminate()
	{
		m_rootSignature.Reset();
		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
